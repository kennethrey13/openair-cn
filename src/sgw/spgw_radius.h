// radcli reference guide: http://radcli.github.io/radcli/manual/group__radcli-api.html
/* TODO:
- Add config options in spgw_config.h
- Find a better way to edit radiusclient.conf
- Test with a working haulage
*/

#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <syslog.h>
#include <string.h>
#include <radcli/radcli.h>

#include "spgw_config.h"

#ifndef SPGW_RADIUS_H_
#define SPGW_RADIUS_H_

#define SPGW_RADIUS_CONFIG_FILE "/etc/radcli/radiusclient.conf"
/* The following settings should be used for the conf file:
  authserver has the format <address>:<port>:<secret>

  ---
  authserver 	localhost:1812:spgw_radius_secret
  acctserver 	localhost
  dictionary 	/etc/radcli/dictionary
  default_realm
  radius_timeout 2
  radius_retries 1
  ---

*/

// Message parameters
#define SPGW_RADIUS_CLIENT_PORT 0 // 0 is any available port
// #define SPGW_RADIUS_AUTH_USERNAME "spgw_radius"
// #define SPGW_RADIUS_AUTH_PASSWORD "spgw_radius_password"
#define SPGW_RADIUS_IPV4_REQUEST_TYPE PW_USER_NAME
#define SPGW_RADIUS_IPV4_RESPONSE_TYPE PW_FRAMED_IP_ADDRESS 
// #define SPGW_RADIUS_IPV4_RELEASE_TYPE PW_CALLBACK_LOGIN

// Print enables
#define SPGW_RADIUS_MESSAGE_PRINTING
#define SPGW_RADIUS_DEBUG_PRINTING
#define SPGW_RADIUS_ERROR_PRINTING



/**** Radius Functions ****/
/*
 * Functions for communicating via radius
 * All functions are blocking
 * MUST use configure before sending messages
 * MUST NOT call the initial configure in threads
 * - Only a single thread should initialize config, since the lock 
 *   is not initialized yet
 * - After first config call, any thread can call config safely
 * 
 * To build and send a message:
 * - Start by calling add_attribute with a null value for VALUE_PAIR *send
 * - Continue using add_attribute until all attributes are added
 * - Use send_message, which blocks until a VALUE_PAIR reply is back
 * - Use functions (see handle_ipv4_address) to get result
 */

// Adds an attribute to a provided message (or creates one if *send is NULL)
// Returns 0 on success, -1 on failure
int spgw_radius_add_attribute(VALUE_PAIR **send, rc_attr_id type, const void *content);

// Tests if a connection can be established using simple message
// Works with ANY response, including a reject
// Sets spgw_radius_connection, which can be used to check status
//   of most recent connection (use spgw_radius_connected() to check)
// Returns 0 on success, -1 on failure
int spgw_radius_test_connection(void);

// Sends a radius message and waits for response if configured
// Returns response on success, NULL on failure
VALUE_PAIR *spgw_radius_send_message(VALUE_PAIR *send);

// Handles radius messages for ipv4 addresses
// Returns 0 on success, -1 on failue
int spgw_radius_handle_ipv4_address(const char *imsi, struct in_addr *addr);

// Sends a radius message to request an ip address
// Returns 0 on success, -1 on failue
int spgw_radius_request_ipv4_address(const char *imsi, struct in_addr *addr);

// Sends a radius message to release an ip address
// Returns 0 on success, -1 on failue
int spgw_radius_release_ipv4_address(const char *imsi, struct in_addr *addr);

// Cleans up all allocated memory, except lock
// Resets config struct and stops messages from being sent
//   until configure is called again
void spgw_radius_clean(void);

// Configures the radius client with default configurations
// Allows messages to be sent until clean is called
// Does nothing if already configured (until clean is called)
int spgw_radius_configure(spgw_config_t *spgw_conf);

// Returns the configuration status
bool spgw_radius_is_configured(void);

// Returns the connection status
bool spgw_radius_is_connected(void);



/**** Debug/Error Functions ****/
/*
 * Debug and error prints that can be disabled -
 * Comment out the PRINTING defines to disable
 */

// Prints value pair messages
void spgw_radius_message_print(VALUE_PAIR *vp, char *type);

// Prints given message if enabled (see #define above)
void spgw_radius_debug_print(char *message);
void spgw_radius_error_print(char *message);



/**** Test Functions ****/
/*
 * Various testing functions, use main to run test
 */

void *spgw_radius_test_send(void *args); // For thread calling
int spgw_radius_test_main(void); // Runs a sample setup

#endif  // SPGW_RADIUS_H_
