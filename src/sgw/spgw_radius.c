#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>
#include <unistd.h> 
#include <pthread.h> 
#include <time.h>
#include <sys/types.h>
#include <syslog.h>
#include <string.h>
#include <radcli/radcli.h>

#include "spgw_radius.h"

/**** Local State Variables ****/

// Used for configuring and connecting
static bool spgw_radius_initial_config = false;
static bool spgw_radius_configured = false;
static bool spgw_radius_connected = false;

// Radius config struct, used in all message sending
static rc_handle *spgw_radius_rh;

// Lock for sending messages and configuring
static pthread_rwlock_t spgw_radius_lock;



/**** Radius functions ****/

int spgw_radius_add_attribute(VALUE_PAIR **send, rc_attr_id type, const void *content) {
  // Do nothing if not configured
  if (!spgw_radius_is_configured()) {
    spgw_radius_error_print("spgw_radius_add_attribute: ERROR - not configured\n");
    return -1;
  }

  // Append attribute
  if (rc_avpair_add(spgw_radius_rh, send, type, content, -1, 0) == NULL) {
    spgw_radius_error_print("spgw_radius_add_attribute: ERROR - rc_avpair_add failed\n");
    rc_avpair_free(*send);
    return -1;
  }
  return 0;
}


int spgw_radius_test_connection()
{
  // Do nothing if not configured
  if (!spgw_radius_is_configured()) {
    spgw_radius_error_print("spgw_send_test_connection: ERROR - not configured\n");
    return -1;
  }

  spgw_radius_debug_print("spgw_radius_test_connection: attempting connection\n");
  int result;
  VALUE_PAIR *send = NULL, *received = NULL;
  uint32_t service = PW_AUTHENTICATE_ONLY;

  // *** Password not currently used ***
  // if (spgw_radius_add_attribute(&send, PW_USER_NAME, SPGW_RADIUS_AUTH_USERNAME)) {
  //   spgw_radius_connected = false;
  //   spgw_radius_error_print("spgw_radius_test_connection: ERROR - failed to add username attibute\n");
  //   return -1;
  // }
  // if (spgw_radius_add_attribute(&send, PW_USER_PASSWORD, SPGW_RADIUS_AUTH_PASSWORD)) {
  //   spgw_radius_connected = false;
  //   spgw_radius_error_print("spgw_radius_test_connection: ERROR - failed to add password attibute\n");
  //   return -1;
  // }
  if (spgw_radius_add_attribute(&send, PW_SERVICE_TYPE, &service)) {
    spgw_radius_connected = false;
    rc_avpair_free(send);
    spgw_radius_error_print("spgw_radius_test_connection: ERROR - failed to add service attibute\n");
    return -1;
  }

  // Attempt connection via simple authentication message
  pthread_rwlock_rdlock(&spgw_radius_lock);
  result = rc_auth(spgw_radius_rh, SPGW_RADIUS_CLIENT_PORT, send, &received, NULL);
  pthread_rwlock_unlock(&spgw_radius_lock);

  // Response doesn't matter, only need contact
  rc_avpair_free(send);
  rc_avpair_free(received);

  // Check for any server contact
  if (result == OK_RC || result == REJECT_RC) {
    spgw_radius_debug_print("spgw_radius_test_connection: connection success\n");
    spgw_radius_connected = true;
    return 0;
  } else {
    spgw_radius_error_print("spgw_radius_test_connection: ERROR - connection failed\n");
    spgw_radius_connected = false;
    return -1;
  }
}


VALUE_PAIR *spgw_radius_send_message(VALUE_PAIR *send)
{
  // Do nothing if not configured
  if (!spgw_radius_is_configured()) {
    spgw_radius_error_print("spgw_radius_send_message: ERROR - not configured\n");
    return NULL;
  }

  // Get read lock to allow other messages
  pthread_rwlock_rdlock(&spgw_radius_lock);

  // If no connection has been established, try connecting
  if (!spgw_radius_is_connected()) {
    if (spgw_radius_test_connection() < 0) {
      spgw_radius_error_print("spgw_radius_send_message: ERROR - could not connect\n");
      pthread_rwlock_unlock(&spgw_radius_lock);
      return NULL;
    }
  }

  spgw_radius_message_print(send, "sent");

  int result;
  VALUE_PAIR *received = NULL;

  // Send Message
  result = rc_auth(spgw_radius_rh, SPGW_RADIUS_CLIENT_PORT, send, &received, NULL);
  pthread_rwlock_unlock(&spgw_radius_lock);

  if (result == OK_RC) {
    spgw_radius_debug_print("spgw_radius_send_message: message success\n");
    spgw_radius_message_print(received, "received");
    return received;
  } else {
    spgw_radius_error_print("spgw_radius_send_message: ERROR - message failed\n");
    spgw_radius_connected = false; // remove?
    rc_avpair_free(received);
    return NULL;
  }
}


int spgw_radius_handle_ipv4_address(const char *imsi, struct in_addr *addr)
{
  VALUE_PAIR *send = NULL;

  // *** Password not currently used ***
  // if (spgw_radius_add_attribute(&send, PW_USER_NAME, SPGW_RADIUS_AUTH_USERNAME)) {
  //   spgw_radius_error_print("spgw_radius_handle_ipv4_address: ERROR - failed to add username attibute\n");
  //   return -1;
  // }
  // if (spgw_radius_add_attribute(&send, PW_USER_PASSWORD, SPGW_RADIUS_AUTH_PASSWORD)) {
  //   spgw_radius_error_print("spgw_radius_handle_ipv4_address: ERROR - failed to add password attibute\n");
  //   return -1;
  // }

  if (spgw_radius_add_attribute(&send, PW_USER_NAME, imsi)) {
    spgw_radius_error_print("spgw_radius_handle_ipv4_address: ERROR - failed to add id attibute\n");
    return -1;
  }

  VALUE_PAIR *received = spgw_radius_send_message(send);

  int res = 0;

  // Look for proper response attribute
  VALUE_PAIR *response = rc_avpair_get(received, SPGW_RADIUS_IPV4_RESPONSE_TYPE, 0);
  if (response != NULL) {
    if (rc_avpair_get_uint32(response, &(addr->s_addr)) < 0) {
      res = -1;
      spgw_radius_error_print("spgw_radius_handle_ipv4_address: ERROR - failed to extract response\n");
    }

    // Response recieved
    spgw_radius_debug_print("spgw_radius_handle_ipv4_address: ip address received\n");

  } else {
    res = -1;
    spgw_radius_error_print("spgw_radius_handle_ipv4_address: ERROR - no response received\n");
  }

  rc_avpair_free(send);
  rc_avpair_free(received);
  return res;
}


int spgw_radius_request_ipv4_address(const char *imsi, struct in_addr *addr)
{
  if (spgw_radius_handle_ipv4_address(imsi, addr) < 0) {
    spgw_radius_error_print("spgw_radius_request_ipv4_address: ERROR - failed to send message\n");
    return -1;
  }
  return 0;
}


int spgw_radius_release_ipv4_address(const char *imsi, struct in_addr *addr)
{
  // *** Release not currently used ***
  // if (spgw_radius_handle_ipv4_address(imsi, addr) < 0) {
  //   spgw_radius_error_print("spgw_radius_release_ipv4_address: ERROR - failed to send message\n");
  // }
  return 0;
}


void spgw_radius_clean(void)
{
  // Get write lock to ensure no new messages are sent
  pthread_rwlock_wrlock(&spgw_radius_lock);
  spgw_radius_debug_print("spgw_radius_clean: cleaning up\n");

  // Clean up
  spgw_radius_configured = false;
  spgw_radius_connected = false;

  if (spgw_radius_rh != NULL) {
    rc_destroy(spgw_radius_rh);
    spgw_radius_rh = NULL;
  }

  pthread_rwlock_unlock(&spgw_radius_lock);
}


int spgw_radius_configure(void)
{
  // Do nothing if already configured
  if (spgw_radius_is_configured()) {
    spgw_radius_debug_print("spgw_radius_configure: already configured\n");
    return 0;
  }

  // Initial configs here
  if (!spgw_radius_initial_config) {
    spgw_radius_debug_print("spgw_radius_configure: performing initial config\n");
    if(pthread_rwlock_init(&spgw_radius_lock, NULL) != 0) {
      spgw_radius_error_print("spgw_radius_configure: ERROR - failed to initialize lock\n");
      return -1;
    } else {
      spgw_radius_debug_print("spgw_radius_configure: lock Initialized\n");
    }
    spgw_radius_initial_config = true;
  }

  // Write lock to ensure no message sent with bad configuration
  pthread_rwlock_wrlock(&spgw_radius_lock);
  spgw_radius_debug_print("spgw_radius_configure: configuring spgw_radius\n");
  
  // Read in defaults from config file
  if ((spgw_radius_rh = rc_read_config(SPGW_RADIUS_CONFIG_FILE)) == NULL) {
    spgw_radius_error_print("spgw_radius_configure: ERROR - unable to read in config file\n");
    pthread_rwlock_unlock(&spgw_radius_lock);
    return -1;
  }

  spgw_radius_debug_print("spgw_radius_configure: configure successful\n");
  spgw_radius_configured = true;
  pthread_rwlock_unlock(&spgw_radius_lock);
  return 0;
}


bool spgw_radius_is_configured(void)
{
  return spgw_radius_configured;
}


bool spgw_radius_is_connected(void)
{
  return spgw_radius_connected;
}



/**** Debug/Error Functions ****/

void spgw_radius_message_print(VALUE_PAIR *vp, char *type) 
{ 
  #ifdef SPGW_RADIUS_MESSAGE_PRINTING
  char name[128];
  char value[128];

  printf("\n------------------------\n");
  printf("Message(%s):\n", type);
  while(vp != NULL) {
    if (rc_avpair_tostr(spgw_radius_rh, vp, name, sizeof(name), value, sizeof(value)) == 0) {
      printf(" - %s:\t%s\n", name, value);
    }
    vp = vp->next;
  }
  printf("\n------------------------\n");
  #endif
}


void spgw_radius_debug_print(char *message) {
  #ifdef SPGW_RADIUS_DEBUG_PRINTING
  printf("%s", message);
  #endif
}


void spgw_radius_error_print(char *message) {
  #ifdef SPGW_RADIUS_ERROR_PRINTING
  printf("%s", message);
  #endif
}



/**** Test Functions ****/

void *spgw_radius_test_send(void *args) {
  char message[20];
  long thread_id = (long) args;
  snprintf(message, 20, "Message(%ld)", thread_id);
  if (spgw_radius_configure() == 0) {
    if (spgw_radius_test_connection() == 0) {

      // Send 2-4 messages
      for (int i = 0; i < (thread_id % 3) + 2; i++) {
        // Test multiple clears and configures in between messages
        if ((long) args % 5 == 0 && i < 2) {
          spgw_radius_clean();
        } else if ((long) args % 10 == 1) {
          spgw_radius_configure();
        }

        struct in_addr addr;
        spgw_radius_request_ipv4_address(message, &addr);

        // Delay 0-3 seconds
        sleep(((thread_id+i) % 4));
      }
    } else {
      printf("spgw_radius_test_send: failed to connect\n");
      spgw_radius_clean();
      return NULL;
    }
  } else {
    printf("spgw_radius_test_send: failed to configure\n");
    spgw_radius_clean();
    return NULL;
  }
  printf("spgw_radius_test_send: thread %ld finished\n", thread_id);
  return NULL;
}


int spgw_radius_test_main(void)
{
  setvbuf(stdout, NULL, _IONBF, 0); // Disable buffering
  printf("spgw_radius_test_main: starting program\n");

  // Initial config
  spgw_radius_configure();
  spgw_radius_test_connection();

  int num_threads = 50;
  printf("spgw_radius_test_main: creating %d threads\n", num_threads);

  pthread_t threads[num_threads];
  for (long i = 0; i < num_threads; i++)
    pthread_create(&threads[i], NULL, spgw_radius_test_send, (void *) i);

  for (long i = 0; i < num_threads; i++)
    pthread_join(threads[i], NULL);

  // Clean up and close
  spgw_radius_clean();
  printf("spgw_radius_test_main: exiting normally\n");
  return 0;
}
