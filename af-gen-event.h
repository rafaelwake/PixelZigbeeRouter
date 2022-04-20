// This file is generated by Simplicity Studio.  Please do not edit manually.
//
//

// Enclosing macro to prevent multiple inclusion
#ifndef __AF_GEN_EVENT__
#define __AF_GEN_EVENT__


// Code used to configure the cluster event mechanism
#define EMBER_AF_GENERATED_EVENT_CODE \
  extern EmberEventControl emberAfPluginConnectionManagerPollEventControl; \
  extern EmberEventControl emberAfPluginConnectionManagerRebootEventControl; \
  extern EmberEventControl emberAfPluginConnectionManagerRejoinEventControl; \
  extern EmberEventControl emberAfPluginManufacturingLibraryCliCheckSendCompleteEventControl; \
  extern EmberEventControl emberAfPluginNetworkSteeringFinishSteeringEventControl; \
  extern EmberEventControl emberAfPluginScanDispatchScanEventControl; \
  extern EmberEventControl emberAfPluginUpdateTcLinkKeyBeginTcLinkKeyUpdateEventControl; \
  extern EmberEventControl inputActionEventControl; \
  extern EmberEventControl ledEventControl; \
  extern EmberEventControl reportStatusEventControl; \
  extern void emberAfPluginConnectionManagerPollEventHandler(void); \
  extern void emberAfPluginConnectionManagerRebootEventHandler(void); \
  extern void emberAfPluginConnectionManagerRejoinEventHandler(void); \
  extern void emberAfPluginManufacturingLibraryCliCheckSendCompleteEventHandler(void); \
  extern void emberAfPluginNetworkSteeringFinishSteeringEventHandler(void); \
  extern void emberAfPluginScanDispatchScanEventHandler(void); \
  extern void emberAfPluginUpdateTcLinkKeyBeginTcLinkKeyUpdateEventHandler(void); \
  extern void inputActionEventFunction(void); \
  extern void ledEventFunction(void); \
  extern void reportStatusEventFunction(void); \


// EmberEventData structs used to populate the EmberEventData table
#define EMBER_AF_GENERATED_EVENTS   \
  { &emberAfPluginConnectionManagerPollEventControl, emberAfPluginConnectionManagerPollEventHandler }, \
  { &emberAfPluginConnectionManagerRebootEventControl, emberAfPluginConnectionManagerRebootEventHandler }, \
  { &emberAfPluginConnectionManagerRejoinEventControl, emberAfPluginConnectionManagerRejoinEventHandler }, \
  { &emberAfPluginManufacturingLibraryCliCheckSendCompleteEventControl, emberAfPluginManufacturingLibraryCliCheckSendCompleteEventHandler }, \
  { &emberAfPluginNetworkSteeringFinishSteeringEventControl, emberAfPluginNetworkSteeringFinishSteeringEventHandler }, \
  { &emberAfPluginScanDispatchScanEventControl, emberAfPluginScanDispatchScanEventHandler }, \
  { &emberAfPluginUpdateTcLinkKeyBeginTcLinkKeyUpdateEventControl, emberAfPluginUpdateTcLinkKeyBeginTcLinkKeyUpdateEventHandler }, \
  { &inputActionEventControl, inputActionEventFunction }, \
  { &ledEventControl, ledEventFunction }, \
  { &reportStatusEventControl, reportStatusEventFunction }, \


#define EMBER_AF_GENERATED_EVENT_STRINGS   \
  "Connection Manager Plugin Poll",  \
  "Connection Manager Plugin Reboot",  \
  "Connection Manager Plugin Rejoin",  \
  "Manufacturing Library CLI Plugin CheckSendComplete",  \
  "Network Steering Plugin FinishSteering",  \
  "Scan Dispatch Plugin Scan",  \
  "Update TC Link Key Plugin BeginTcLinkKeyUpdate",  \
  "Input action event control",  \
  "Led event control",  \
  "Report status event control",  \


#endif // __AF_GEN_EVENT__