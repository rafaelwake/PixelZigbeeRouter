/***************************************************************************//**
 * @file
 * @brief Callback implementation for Zigbee Router sample application.
 *******************************************************************************
 * # Author
 * <b>Roberto Luiz Assad Pinheiro - roberto.pinheiro@pixelti.com.br</b>
 *******************************************************************************/


// This callback file is created for your convenience. You may add application
// code to this file. If you regenerate this file over a previous version, the
// previous version will be overwritten and any code you have added will be
// lost.

#include "app/framework/include/af.h"

/*** Input Pins ***/
#define RESET 								BUTTON0
/*** Output Pins ***/
#define LED_STATUS 							BOARDLED1
#define LED_OUTPUT							BOARDLED2
/*** ZigBee Constants ***/
#define DEVICE_ENDPOINT (1)
#define GATEWAY_ENDPOINT (8)
/*** Timers and Delays, Resets ***/
#define SEC_RST_ZIG 						4		//seconds to reset zigbee
#define SEC_BLINK_LED 						3		//seconds to blink the led fast
#define PERIODICALLY_REPORT_STATE			300000  //Periodically report
/*** LED ***/
#define LED_LONG_ON_DELAY					3000
#define LED_LONG_BLINK_PERIOD_MS			700
#define LED_SHORT_BLINK_PERIOD_MS 			200
#define LED_BLINK_TIMES_WHEN_START			3
#define LED_BLINK_TIMES_FAST				(30000 / LED_SHORT_BLINK_PERIOD_MS)
#define	LED_BLINK_TIMES_MACRO(times)		(times * 2)

/*** State ***/
#define ON 									0x10
#define OFF 								0xEF


EmberEventControl inputActionEventControl;
EmberEventControl ledEventControl;
EmberEventControl reportStatusEventControl;
EmberEventControl delayLed;

/*** Status vars ***/
static uint8_t statusOnOff;
static uint8_t statusBlink;
/*** Control vars ***/
/*** Button ***/
static bool resetCounter = false;
static bool ledStartBlinkCounter = false;
static bool ledOnOffCounter = false;
static bool buttonPressed = false;
/*** LED ***/
static bool ledLeavingNetwork = false;
static bool ledStopSearchNetwork = false;
static bool ledOnAction = false;
static uint8_t ledStartBlinkTimes = LED_BLINK_TIMES_MACRO(LED_BLINK_TIMES_WHEN_START);
static uint8_t ledFastBlinkTimes = LED_BLINK_TIMES_MACRO(LED_BLINK_TIMES_FAST);


/*** Reset ***/
void resetZigbee(void);
/*** Send Data ***/
void sendStateFunction(void);
/*** Output Controllers ***/
void outputController(void); //On Off Led
void outputControllerBlinkLed(void); //Blink Led


//------------------------------------------------------------------------------
/*
 * Event Handler
 */

/*********************************************************************
 *
 * @fn      inputActionEventFunction(void)
 *
 * @brief   Event function to control the inputs
 *
 * @param   none
 *
 * @return  none
 */
void inputActionEventFunction(void)
{
	emberEventControlSetInactive(inputActionEventControl);

	emberAfCorePrintln(">>>>>>>> RESET COUNTER: %d");
	emberAfCorePrintln(">>>>>>>> STATUS BUTTON: %d", buttonPressed);
	emberAfCorePrintln(">>>>>>>> BLINK LED: %d", ledStartBlinkCounter);
	emberAfCorePrintln(">>>>>>>> Led status On or Off: %d", ledOnOffCounter);


	if(buttonPressed){

		if (ledStartBlinkCounter != true){
			//Start 3 seconds timer
			ledStartBlinkCounter = true;
			emberEventControlSetDelayMS(inputActionEventControl, SEC_BLINK_LED*1000);
		}
		else{
			if (resetCounter != true)
			{
				//Start 7 seconds timer
				resetCounter = true;
				emberEventControlSetDelayMS(inputActionEventFunction, SEC_RST_ZIG*1000);
			} else
			{
				//Reset Device
				emberAfCorePrintln(">>>>>>>> RESET DEVICE");
				resetZigbee();
				resetCounter = false;
				ledStartBlinkCounter = false;
				ledOnOffCounter = true;
				statusBlink = OFF;
			}
		}
	}else /* if button is unpressed < 3 or < 7 seconds */
	{
		if (ledOnOffCounter != true && resetCounter)
		{
			statusBlink = ON;
			outputControllerBlinkLed();
			resetCounter = false;
		}
		else if(ledOnOffCounter != true && ledStartBlinkCounter){
			statusBlink = OFF;
			emberAfAppPrintln("\r\n");
			if (statusOnOff == ON) {
				statusOnOff = OFF;
				emberAfAppPrintln(">>> OFF <<<");
			} else {
				statusOnOff = ON;
				emberAfAppPrintln(">>> ON <<<");
			}
			emberAfAppPrintln("\r\n");
			outputController();
			ledStartBlinkCounter = false;
		}
	}


	if(emberAfNetworkState() == EMBER_JOINED_NETWORK_NO_PARENT)
		emberAfPluginConnectionManagerRejoinEventHandler();

}

/*********************************************************************
 *
 * @fn      ledEventFunction(void)
 *
 * @brief   Event function to control the leds on device
 *
 * @param   none
 *
 * @return  none
 */
void ledEventFunction(void)
{
	emberEventControlSetInactive(ledEventControl);
	uint8_t networkState = emberAfNetworkState();

	if(ledStartBlinkTimes == 0) {
		halClearLed(LED_STATUS);
		ledStartBlinkTimes = LED_BLINK_TIMES_MACRO(LED_BLINK_TIMES_WHEN_START);
		ledOnAction = false;
		return;
	}

	if(ledFastBlinkTimes == 0) {
		halClearLed(LED_STATUS);
		ledFastBlinkTimes = LED_BLINK_TIMES_MACRO(LED_BLINK_TIMES_FAST);
		ledOnAction = false;
		return;
	}

	if(ledLeavingNetwork) {
		//emberAfCorePrintln(">>>>>>>>>>>>>>	LED:      EMBER_LEAVING_NETWORK");
		ledOnAction = true;
		ledLeavingNetwork = false;
		halSetLed(LED_STATUS);
		emberEventControlSetDelayMS(ledEventControl, LED_LONG_ON_DELAY);
	} else
		if ((networkState == EMBER_NO_NETWORK || networkState == EMBER_NO_NETWORK) && ledStopSearchNetwork == false) {
			//emberAfCorePrintln(">>>>>>>>>>>>>>	LED:      EMBER_NO_NETWORK");
			ledOnAction = true;
			halToggleLed(LED_STATUS);
			emberEventControlSetDelayMS(ledEventControl, LED_LONG_BLINK_PERIOD_MS);
		} else
			if ((networkState == EMBER_JOINED_NETWORK || networkState == EMBER_JOINING_NETWORK) && ledOnAction == true && ledFastBlinkTimes > 0) {
				//emberAfCorePrintln(">>>>>>>>>>>>>>	LED:      EMBER_JOINING_NETWORK  %d", ledFastBlinkTimes);
				ledOnAction = true;
				halToggleLed(LED_STATUS);
				ledFastBlinkTimes--;
				emberEventControlSetDelayMS(ledEventControl, LED_SHORT_BLINK_PERIOD_MS);
			} else
				if(networkState == EMBER_JOINED_NETWORK && ledOnAction == false && ledStartBlinkTimes > 0) {
					//emberAfCorePrintln(">>>>>>>>>>>>>>	LED:      EMBER_JOINED_NETWORK");
					halToggleLed(LED_STATUS);
					ledStartBlinkTimes--;
					emberEventControlSetDelayMS(ledEventControl, LED_SHORT_BLINK_PERIOD_MS);
				} else {
					//emberAfCorePrintln(">>>>>>>>>>>>>>	LED:      OFF");
					halClearLed(LED_STATUS);
					ledOnAction = false;
					ledStopSearchNetwork = false;
				}
}

/*********************************************************************
 * @fn      reportStatusEventFunction(void)
 *
 * @brief   Event function to periodically report
 *
 * @param   none
 *
 * @return  none
 */
void reportStatusEventFunction(void)
{
	sendStateFunction();
	emberEventControlSetDelayMS(reportStatusEventControl, PERIODICALLY_REPORT_STATE);
}

//------------------------------------------------------------------------------
/*
 * Custom Function
 */

/*********************************************************************
 * @fn      sendStateFunction(void)
 *
 * @brief   Send the actual state and save on token
 *
 * @param   none
 *
 * @return  none
 */
void sendStateFunction(void)
{
	uint8_t state;
	if(statusOnOff == ON)
		state = 0x01;
	else
		state = 0x00;

	//uint8_t forSave = generateFirstDataByte();
	emberAfWriteServerAttribute(DEVICE_ENDPOINT,
			ZCL_ON_OFF_CLUSTER_ID,
			ZCL_ON_OFF_ATTRIBUTE_ID,
			(uint8_t *)&state,
			ZCL_BOOLEAN_ATTRIBUTE_TYPE);

}

/*********************************************************************
 * @fn      resetZigbee(void)
 *
 * @brief   Make the device leave the network and put it in permit join
 *
 * @param   none
 *
 * @return  none
 */
void resetZigbee(void)
{
	emberAfCorePrintln("\r\n");
	emberAfCorePrintln("------------------------------------- {");
	emberAfCorePrintln("	resetFunction");

	emberAfPluginNetworkSteeringStop();

	emberLeaveNetwork();
	emberAfPluginConnectionManagerLeaveNetworkAndStartSearchForNewOne();
	emberAfPluginConnectionManagerFactoryReset();

	ledLeavingNetwork = true;
	emberEventControlSetActive(ledEventControl);

	emberAfCorePrintln("------------------------------------- }");
	emberAfCorePrintln("\r\n");
}

/*********************************************************************
 * @fn      outputController()
 *
 * @brief   Control function to output pin for action
 *
 * @param   none
 *
 * @return  none
 */
void outputController(void)
{
	if(statusOnOff == ON) {
		halSetLed(LED_OUTPUT);
	} else {
		halClearLed(LED_OUTPUT);
	}
}


/*********************************************************************
 * @fn      outputControllerBlinkLed()
 *
 * @brief   Control function to output pin for action
 * @Led is bliking 300ms bettwen On or Off
 * @param   none
 *
 * @return  none
 */
void outputControllerBlinkLed(void)
{

	if (statusBlink == ON) {

		if (statusOnOff == OFF) {
			halSetLed(LED_OUTPUT);
		}else{
			halClearLed(LED_OUTPUT);
		}
	}

}

//------------------------------------------------------------------------------


/** @brief Main Init
 *
 * This function is called from the application's main function. It gives the
 * application a chance to do any initialization required at system startup.
 * Any code that you would normally put into the top of the application's
 * main() routine should be put into this function.
        Note: No callback
 * in the Application Framework is associated with resource cleanup. If you
 * are implementing your application on a Unix host where resource cleanup is
 * a consideration, we expect that you will use the standard Posix system
 * calls, including the use of atexit() and handlers for signals such as
 * SIGTERM, SIGINT, SIGCHLD, SIGPIPE and so on. If you use the signal()
 * function to register your signal handler, please mind the returned value
 * which may be an Application Framework function. If the return value is
 * non-null, please make sure that you call the returned function from your
 * handler to avoid negating the resource cleanup of the Application Framework
 * itself.
 *
 */
void emberAfMainInitCallback(void)
{
	emberEventControlSetDelayMS(reportStatusEventControl, PERIODICALLY_REPORT_STATE);
}

/** @brief Stack Status
 *
 * This function is called by the application framework from the stack status
 * handler.  This callbacks provides applications an opportunity to be notified
 * of changes to the stack status and take appropriate action.  The return code
 * from this callback is ignored by the framework.  The framework will always
 * process the stack status after the callback returns.
 *
 * @param status   Ver.: always
 */
bool emberAfStackStatusCallback(EmberStatus status)
{
	emberAfCorePrintln("\r\n");
	emberAfCorePrintln("-------------------------------------");
	emberAfCorePrintln("Stack Status Callback");
	switch (status)
	{
	case EMBER_NETWORK_DOWN:
		emberAfAppPrintln(" | NETWORK DOWN");
		break;
	case EMBER_NETWORK_UP:
		emberAfAppPrintln(" | NETWORK UP");
		uint16_t identifyTime;
		emberAfReadServerAttribute(DEVICE_ENDPOINT,
				ZCL_IDENTIFY_CLUSTER_ID,
				ZCL_IDENTIFY_TIME_ATTRIBUTE_ID,
				(uint8_t *)&identifyTime,
				sizeof(identifyTime));
		break;
	}
	emberAfCorePrintln("-------------------------------------");
	emberAfCorePrintln("\r\n");

	emberEventControlSetActive(ledEventControl);

	return false;
}

/** @brief Begin searching for network to join
 *
 * This function is called by the Connection Manager Plugin when it starts
 * to search for a new network.  It is normally used to trigger a UI event to
 * notify the user that the device is currently searching for a network.
 */
void emberAfPluginConnectionManagerLeaveNetworkCallback(void)
{
	emberAfAppPrintln("\r\n");
	emberAfAppPrintln("------------------------------------- {");
	emberAfAppPrintln(" | emberAfPluginConnectionManagerLeaveNetworkCallback");
	emberAfAppPrintln(" | START NTWK LEAVE");
	emberAfAppPrintln("------------------------------------- }");
	emberAfAppPrintln("\r\n");

	ledLeavingNetwork = true;
	emberEventControlSetDelayMS(ledEventControl, 100);
}

/** @brief Network join finished
 *
 * This callback is fired when the Connection Manager plugin is finished with
 * the network search process. The result of the operation will be returned as
 * the status parameter.
 *
 * @param status   Ver.: always
 */
void emberAfPluginConnectionManagerFinishedCallback(EmberStatus status)
{
	emberAfCorePrintln("\r\n");
	emberAfCorePrintln("-------------------------------------");
	emberAfCorePrintln("Connection Manager Finished Callback");

	if (status == EMBER_NETWORK_UP) {
		emberAfCorePrintln(" | SUCCESS");
	} else {
		emberAfCorePrintln(" | FAIL");
		ledStopSearchNetwork = true;
	}

	emberEventControlSetActive(ledEventControl);

	emberAfCorePrintln("-------------------------------------");
	emberAfCorePrintln("\r\n");
}

/** @brief On/off Cluster Server Post Init
 *
 * Following resolution of the On/Off state at startup for this endpoint, perform any
 * additional initialization needed; e.g., synchronize hardware state.
 *
 * @param endpoint Endpoint that is being initialized  Ver.: always
 */
void emberAfPluginOnOffClusterServerPostInitCallback(uint8_t endpoint)
{
	// At startup, trigger a read of the attribute and possibly a toggle of the
	// LED to make sure they are always in sync.
	emberAfOnOffClusterServerAttributeChangedCallback(endpoint,
			ZCL_ON_OFF_ATTRIBUTE_ID);
}

/** @brief Server Attribute Changed
 *
 * On/off cluster, Server Attribute Changed
 *
 * @param endpoint Endpoint that is being initialized  Ver.: always
 * @param attributeId Attribute that changed  Ver.: always
 */
void emberAfOnOffClusterServerAttributeChangedCallback(uint8_t endpoint,
		EmberAfAttributeId attributeId)
{
	if (attributeId == ZCL_ON_OFF_ATTRIBUTE_ID) {
		extern uint8_t appZclBuffer[255];
		extern EmberApsFrame globalApsFrame;
		extern uint16_t appZclBufferLen;
		uint8_t data;
		uint8_t size;
		EmberAfAttributeType type = ZCL_BOOLEAN_ATTRIBUTE_TYPE;
		if (emberAfReadServerAttribute(endpoint,
				ZCL_ON_OFF_CLUSTER_ID,
				ZCL_ON_OFF_ATTRIBUTE_ID,
				&data,
				sizeof(data))
				== EMBER_ZCL_STATUS_SUCCESS) {

			zclBufferSetup(ZCL_GLOBAL_COMMAND | (ZCL_FRAME_CONTROL_SERVER_TO_CLIENT), ZCL_ON_OFF_CLUSTER_ID, ZCL_REPORT_ATTRIBUTES_COMMAND_ID);
			zclBufferAddWord(ZCL_ON_OFF_ATTRIBUTE_ID);
			zclBufferAddByte(type);
			size = emberAfGetDataSize(type);
			zclBufferAddBuffer(&data,size);
			emAfApsFrameEndpointSetup(endpoint, GATEWAY_ENDPOINT);
			emberAfSendUnicast(EMBER_OUTGOING_DIRECT, 0, &globalApsFrame, appZclBufferLen, appZclBuffer);

			emberAfAppPrintln("\r\n");
			if (data) {
				statusOnOff = ON;
				emberAfAppPrintln(">>> ON <<<");
			} else {
				statusOnOff = OFF;
				emberAfAppPrintln(">>> OFF <<<");
			}
			emberAfAppPrintln("\r\n");

			outputController();
		}
	}
}

/** @brief Hal Button Isr
 *
 * This callback is called by the framework whenever a button is pressed on the
 * device. This callback is called within ISR context.
 *
 * @param button The button which has changed state, either BUTTON0 or BUTTON1
 * as defined in the appropriate BOARD_HEADER.  Ver.: always
 * @param state The new state of the button referenced by the button parameter,
 * either ::BUTTON_PRESSED if the button has been pressed or ::BUTTON_RELEASED
 * if the button has been released.  Ver.: always
 */
void emberAfHalButtonIsrCallback(int8u button,
		int8u state)
{
	buttonPressed = state;
	emberEventControlSetActive(inputActionEventControl);
}
