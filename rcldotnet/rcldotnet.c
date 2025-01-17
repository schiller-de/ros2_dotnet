// Copyright 2016-2018 Esteve Fernandez <esteve@apache.org>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <assert.h>
#include <stdlib.h>

#include <rcl/error_handling.h>
#include <rcl/node.h>
#include <rcl/rcl.h>
#include <rmw/rmw.h>

#include "rosidl_runtime_c/message_type_support_struct.h"

#include "rcldotnet.h"

static rcl_context_t context;

int32_t native_rcl_init() {
  // TODO(esteve): parse args
  int num_args = 0;
  context = rcl_get_zero_initialized_context();
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
  rcl_ret_t ret = rcl_init_options_init(&init_options, allocator);
  if (RCL_RET_OK != ret) {
    return ret;
  }
  const char ** arg_values = NULL;
  ret = rcl_init(num_args, arg_values, &init_options, &context);
  return ret;
}

const char *native_rcl_get_rmw_identifier() {
  return rmw_get_implementation_identifier();
}

void native_rcl_get_error_string(char *buffer, int32_t bufferSize) {
  size_t minBufferSize = (size_t)bufferSize < (size_t)RCUTILS_ERROR_MESSAGE_MAX_LENGTH
    ? (size_t)bufferSize
    : (size_t)RCUTILS_ERROR_MESSAGE_MAX_LENGTH;

  strncpy(buffer, rcl_get_error_string().str, minBufferSize);
}

void native_rcl_reset_error(void) {
  rcl_reset_error();
}

int32_t native_rcl_ok() {
  bool result = rcl_context_is_valid(&context);
  return result ? 1 : 0;
}

int32_t native_rcl_create_node_handle(void **node_handle, const char *name, const char *namespace) {
  rcl_node_t *node = (rcl_node_t *)malloc(sizeof(rcl_node_t));
  *node = rcl_get_zero_initialized_node();

  rcl_node_options_t default_options = rcl_node_get_default_options();
  rcl_ret_t ret = rcl_node_init(node, name, namespace, &context, &default_options);
  *node_handle = (void *)node;
  return ret;
}

int32_t native_rcl_destroy_node_handle(void *node_handle) {
  rcl_node_t *node = (rcl_node_t *)node_handle;

  rcl_ret_t ret = rcl_node_fini(node);
  free(node);

  return ret;
}

int32_t native_rcl_create_guard_condition_handle(void **guard_condition_handle) {
  rcl_guard_condition_t *guard_condition =
      (rcl_guard_condition_t *)malloc(sizeof(rcl_guard_condition_t));
  *guard_condition = rcl_get_zero_initialized_guard_condition();
  rcl_guard_condition_options_t guard_condition_ops =
      rcl_guard_condition_get_default_options();

  rcl_ret_t ret =
      rcl_guard_condition_init(guard_condition, &context, guard_condition_ops);

  *guard_condition_handle = (void *)guard_condition;

  return ret;
}

int32_t native_rcl_destroy_guard_condition_handle(void *guard_condition_handle) {
  rcl_guard_condition_t *guard_condition = (rcl_guard_condition_t *)guard_condition_handle;

  rcl_ret_t ret = rcl_guard_condition_fini(guard_condition);
  free(guard_condition);

  return ret;
}

int32_t native_rcl_create_wait_set_handle(
    void **wait_set_handle,
    int32_t number_of_subscriptions,
    int32_t number_of_guard_conditions,
    int32_t number_of_timers,
    int32_t number_of_clients,
    int32_t number_of_services,
    int32_t number_of_events) {
  rcl_wait_set_t *wait_set = (rcl_wait_set_t *)malloc(sizeof(rcl_wait_set_t));
  *wait_set = rcl_get_zero_initialized_wait_set();

  rcl_ret_t ret = rcl_wait_set_init(
      wait_set, number_of_subscriptions, number_of_guard_conditions,
      number_of_timers, number_of_clients, number_of_services,
      number_of_events, &context, rcl_get_default_allocator());

  *wait_set_handle = (void *)wait_set;

  return ret;
}

int32_t native_rcl_destroy_wait_set_handle(void *wait_set_handle) {
  rcl_wait_set_t *wait_set = (rcl_wait_set_t *)wait_set_handle;

  rcl_ret_t ret = rcl_wait_set_fini(wait_set);
  free(wait_set);

  return ret;
}

int32_t native_rcl_wait_set_clear(void *wait_set_handle) {
  rcl_wait_set_t *wait_set = (rcl_wait_set_t *)wait_set_handle;
  rcl_ret_t ret = rcl_wait_set_clear(wait_set);

  return ret;
}

int32_t native_rcl_wait_set_add_subscription(void *wait_set_handle, void *subscription_handle) {
  rcl_wait_set_t *wait_set = (rcl_wait_set_t *)wait_set_handle;
  rcl_subscription_t *subscription = (rcl_subscription_t *)subscription_handle;
  rcl_ret_t ret = rcl_wait_set_add_subscription(wait_set, subscription, NULL);

  return ret;
}

int32_t native_rcl_wait_set_add_service(void *wait_set_handle, void *service_handle) {
  rcl_wait_set_t *wait_set = (rcl_wait_set_t *)wait_set_handle;
  rcl_service_t *service = (rcl_service_t *)service_handle;
  rcl_ret_t ret = rcl_wait_set_add_service(wait_set, service, NULL);

  return ret;
}

int32_t native_rcl_wait_set_add_client(void *wait_set_handle, void *client_handle) {
  rcl_wait_set_t *wait_set = (rcl_wait_set_t *)wait_set_handle;
  rcl_client_t *service = (rcl_client_t *)client_handle;
  rcl_ret_t ret = rcl_wait_set_add_client(wait_set, service, NULL);

  return ret;
}

int32_t native_rcl_wait_set_add_guard_condition(void *wait_set_handle, void *guard_condition_handle) {
  rcl_wait_set_t *wait_set = (rcl_wait_set_t *)wait_set_handle;
  rcl_guard_condition_t *guard_condition = (rcl_guard_condition_t *)guard_condition_handle;
  rcl_ret_t ret = rcl_wait_set_add_guard_condition(wait_set, guard_condition, NULL);

  return ret;
}

int32_t native_rcl_wait(void *wait_set_handle, int64_t timeout) {
  rcl_wait_set_t *wait_set = (rcl_wait_set_t *)wait_set_handle;
  rcl_ret_t ret = rcl_wait(wait_set, timeout);

  return ret;
}

int32_t native_rcl_wait_set_subscription_ready(void *wait_set_handle, int32_t index) {
  rcl_wait_set_t *wait_set = (rcl_wait_set_t *)wait_set_handle;

  if (index >= wait_set->size_of_subscriptions)
  {
    return false;
  }

  bool result = wait_set->subscriptions[index] != NULL;
  return result ? 1 : 0;
}

int32_t native_rcl_wait_set_client_ready(void *wait_set_handle, int32_t index) {
  rcl_wait_set_t *wait_set = (rcl_wait_set_t *)wait_set_handle;

  if (index >= wait_set->size_of_clients)
  {
    return false;
  }

  bool result = wait_set->clients[index] != NULL;
  return result ? 1 : 0;
}

int32_t native_rcl_wait_set_service_ready(void *wait_set_handle, int32_t index) {
  rcl_wait_set_t *wait_set = (rcl_wait_set_t *)wait_set_handle;

  if (index >= wait_set->size_of_services)
  {
    return false;
  }

  bool result = wait_set->services[index] != NULL;
  return result ? 1 : 0;
}

int32_t native_rcl_wait_set_guard_condition_ready(void *wait_set_handle, int32_t index) {
  rcl_wait_set_t *wait_set = (rcl_wait_set_t *)wait_set_handle;

  if (index >= wait_set->size_of_guard_conditions)
  {
    return false;
  }

  bool result = wait_set->guard_conditions[index] != NULL;
  return result ? 1 : 0;
}

int32_t native_rcl_take(void *subscription_handle, void *message_handle) {
  rcl_subscription_t * subscription = (rcl_subscription_t *)subscription_handle;

  rcl_ret_t ret = rcl_take(subscription, message_handle, NULL, NULL);
  return ret;
}

int32_t native_rcl_create_request_id_handle(void **request_id_handle) {
  rmw_request_id_t *request_id = (rmw_request_id_t *)malloc(sizeof(rmw_request_id_t));
  memset(request_id, 0, sizeof(rmw_request_id_t));

  *request_id_handle = (void *)request_id;
  return RCL_RET_OK;
}

int32_t native_rcl_destroy_request_id_handle(void *request_id_handle) {
  free((rmw_request_id_t *)request_id_handle);
  return RCL_RET_OK;
}

int64_t native_rcl_request_id_get_sequence_number(void *request_id_handle) {
  rmw_request_id_t *request_id = (rmw_request_id_t *)request_id_handle;
  return request_id->sequence_number;
}

int32_t native_rcl_take_request(void *service_handle, void *request_header_handle, void *request_handle) {
  rcl_service_t * service = (rcl_service_t *)service_handle;
  rmw_request_id_t * request_header = (rmw_request_id_t *)request_header_handle;

  rcl_ret_t ret = rcl_take_request(service, request_header, request_handle);
  return ret;
}

int32_t native_rcl_send_response(void *service_handle, void *request_header_handle, void *resopnse_handle) {
  rcl_service_t * service = (rcl_service_t *)service_handle;
  rmw_request_id_t * request_header = (rmw_request_id_t *)request_header_handle;

  rcl_ret_t ret = rcl_send_response(service, request_header, resopnse_handle);
  return ret;
}

int32_t native_rcl_take_response(void *client_handle, void *request_header_handle, void *response_handle) {
  rcl_client_t * client = (rcl_client_t *)client_handle;
  rmw_request_id_t * request_header = (rmw_request_id_t *)request_header_handle;

  rcl_ret_t ret = rcl_take_response(client, request_header, response_handle);
  return ret;
}

int32_t native_rcl_create_qos_profile_handle(void **qos_profile_handle)
{
  rmw_qos_profile_t *qos_profile = (rmw_qos_profile_t *)malloc(sizeof(rmw_qos_profile_t));
  *qos_profile = rmw_qos_profile_default;
  *qos_profile_handle = (void *)qos_profile;

  return RCL_RET_OK;
}

int32_t native_rcl_destroy_qos_profile_handle(void *qos_profile_handle)
{
  rmw_qos_profile_t *qos_profile = (rmw_qos_profile_t *)qos_profile_handle;
  free(qos_profile);

  return RCL_RET_OK;
}

int32_t native_rcl_write_to_qos_profile_handle(
    void *qos_profile_handle,
    int32_t history,
    int32_t depth,
    int32_t reliability,
    int32_t durability,
    uint64_t deadline_sec,
    uint64_t deadline_nsec,
    uint64_t lifespan_sec,
    uint64_t lifespan_nsec,
    int32_t liveliness,
    uint64_t liveliness_lease_duration_sec,
    uint64_t liveliness_lease_duration_nsec,
    int32_t /* bool */ avoid_ros_namespace_conventions)
{
  rmw_qos_profile_t *qos_profile = (rmw_qos_profile_t *)qos_profile_handle;

  // Can't name the enums for both Foxy and Humble the in code.
  // So use implicit conversions for now...
  // This breaking change was introduced in https://github.com/ros2/rmw/commit/05f973575e8e93454e39f51da6227509061ff189
  // In Foxy they are defined as `enum rmw_qos_history_policy_t { ... };
  //   -> so the type is called `enum rmw_qos_history_policy_t`
  // In Humble tey are defined as `typedef enum rmw_qos_history_policy_e { ... } rmw_qos_history_policy_t;
  //   -> so the type is called `rmw_qos_history_policy_t`

  qos_profile->history = /* (rmw_qos_history_policy_t) */ history;
  qos_profile->depth = (size_t)depth;
  qos_profile->reliability = /* (rmw_qos_reliability_policy_t) */ reliability;
  qos_profile->durability = /* (rmw_qos_durability_policy_t) */ durability;
  qos_profile->deadline.sec = deadline_sec;
  qos_profile->deadline.nsec = deadline_nsec;
  qos_profile->lifespan.sec = lifespan_sec;
  qos_profile->lifespan.nsec = lifespan_nsec;
  qos_profile->liveliness = /* (rmw_qos_liveliness_policy_t) */ liveliness;
  qos_profile->liveliness_lease_duration.sec = liveliness_lease_duration_sec;
  qos_profile->liveliness_lease_duration.nsec = liveliness_lease_duration_nsec;
  qos_profile->avoid_ros_namespace_conventions = avoid_ros_namespace_conventions != 0;

  return RCL_RET_OK;
}
