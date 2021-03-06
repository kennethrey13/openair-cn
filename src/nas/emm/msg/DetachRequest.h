/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under 
 * the Apache License, Version 2.0  (the "License"); you may not use this file
 * except in compliance with the License.  
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *-------------------------------------------------------------------------------
 * For more information about the OpenAirInterface (OAI) Software Alliance:
 *      contact@openairinterface.org
 */

#ifndef FILE_DETACH_REQUEST_SEEN
#define FILE_DETACH_REQUEST_SEEN

#include "DetachType.h"
#include "EmmCause.h"
#include "EpsMobileIdentity.h"
#include "MessageType.h"
#include "NasKeySetIdentifier.h"
#include "SecurityHeaderType.h"
#include "3gpp_23.003.h"
#include "3gpp_24.007.h"
#include "3gpp_24.008.h"

/* Minimum length macro. Formed by minimum length of each mandatory field */
#define DETACH_REQUEST_MINIMUM_LENGTH ( \
    DETACH_TYPE_MINIMUM_LENGTH )

/* Maximum length macro. Formed by maximum length of each field */
#define DETACH_REQUEST_MAXIMUM_LENGTH ( \
    DETACH_TYPE_MAXIMUM_LENGTH + \
    NAS_KEY_SET_IDENTIFIER_MAXIMUM_LENGTH + \
    EPS_MOBILE_IDENTITY_MAXIMUM_LENGTH )

# define DETACH_REQUEST_EMM_CAUSE_PRESENT                    (1<<3)

typedef enum detach_request_iei_tag {
  DETACH_REQUEST_EMM_CAUSE_IEI                    = 0x53, /* 0x53 = 83 */
} detach_request_iei;

/*
 * Message name: Detach request
 * Description: This message is sent by the UE to request the release of an EMM context. See table??8.2.11.1.1.
 * Significance: dual
 * Direction: UE to network
 */

typedef struct detach_request_msg_tag {
  /* Mandatory fields */
  eps_protocol_discriminator_t protocoldiscriminator:4;
  security_header_type_t       securityheadertype:4;
  message_type_t               messagetype;
  detach_type_t                   detachtype;
  emm_cause_t                    emmCause;
  NasKeySetIdentifier          naskeysetidentifier;
  eps_mobile_identity_t        gutiorimsi;
  uint32_t                     presencemask;
} detach_request_msg;

int decode_detach_request(detach_request_msg *detachrequest, uint8_t *buffer, uint32_t len);

int encode_detach_request(detach_request_msg *detachrequest, uint8_t *buffer, uint32_t len);

#endif /* ! defined(FILE_DETACH_REQUEST_SEEN) */

