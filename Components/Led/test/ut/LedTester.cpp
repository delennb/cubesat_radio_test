// ======================================================================
// \title  LedTester.cpp
// \author epdal
// \brief  cpp file for Led component test harness implementation class
// ======================================================================

#include "LedTester.hpp"

namespace Components {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  LedTester ::
    LedTester() :
      LedGTestBase("LedTester", LedTester::MAX_HISTORY_SIZE),
      component("Led")
  {
    this->initComponents();
    this->connectPorts();
  }

  LedTester ::
    ~LedTester()
  {

  }

  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------

  void LedTester::testBlinking() {
    // Ensure LED stays off when blinking is disabled
    // The Led component defaults to blinking off
    this->invoke_to_run(0,0); // invoke the 'run' port to simulate running one cycle
    ASSERT_EVENTS_LedState_SIZE(0); // ensure no LedState change events we emitted
    ASSERT_from_gpioSet_SIZE(0); // ensure gpio LED wasn't set
    ASSERT_TLM_LedTransitions_SIZE(0); // ensure no LedTransitions were recorded

    // Send command to enable blinking
    this->sendCmd_BLINKING_ON_OFF(0, 0, Fw::On::ON);
    this->component.doDispatch(); // Trigger execution of async command

    // Step through 3 run cycles to observe LED turning on and off 3 times
    // Cycle 1: LED initalization->On
    this->invoke_to_run(0,0);
    ASSERT_EVENTS_LedState_SIZE(1);
    ASSERT_EVENTS_LedState(0, Fw::On::ON);
    ASSERT_from_gpioSet_SIZE(1);
    ASSERT_from_gpioSet(0, Fw::Logic::HIGH);
    ASSERT_TLM_LedTransitions_SIZE(1);
    ASSERT_TLM_LedTransitions(0, 1);

    // Cycle 2: LED On->Off
    this->invoke_to_run(0,0);
    ASSERT_EVENTS_LedState_SIZE(2);
    ASSERT_EVENTS_LedState(1, Fw::On::OFF);
    ASSERT_from_gpioSet_SIZE(2);
    ASSERT_from_gpioSet(1, Fw::Logic::LOW);
    ASSERT_TLM_LedTransitions_SIZE(2);
    ASSERT_TLM_LedTransitions(1, 2);

    // Cycle 3: LED Off->On
    this->invoke_to_run(0,0);
    ASSERT_EVENTS_LedState_SIZE(3);
    ASSERT_EVENTS_LedState(2, Fw::On::ON);
    ASSERT_from_gpioSet_SIZE(3);
    ASSERT_from_gpioSet(2, Fw::Logic::HIGH);
    ASSERT_TLM_LedTransitions_SIZE(3);
    ASSERT_TLM_LedTransitions(2, 3);
  }

  void LedTester::testBlinkInterval() {
    // Enable LED Blinking
    this->sendCmd_BLINKING_ON_OFF(0, 0, Fw::On::ON);
    this->component.doDispatch(); // Trigger execution of async command

    // Tests blink intervals of 0, 1, ... 9
    U32 blinkInterval;
    for(U32 interval = 0; interval < 10; interval++) {
      blinkInterval = 9 - interval;

      this->paramSet_BLINK_INTERVAL(blinkInterval, Fw::ParamValid::VALID);
      this->paramSend_BLINK_INTERVAL(0, 0);

      // Ensure the LED is on
      ASSERT_EVENTS_LedState_SIZE(interval+1);
      ASSERT_EVENTS_LedState(interval, FW::On::ON);
      ASSERT_from_gpioSet_SIZE(interval+1);
      ASSERT_from_gpioSet(interval, Fw::Logic::HIGH);

      // Ensure the LED has the correct blink interval
      ASSERT_EVENTS_BlinkIntervalSet_SIZE(interval+1);
      ASSERT_EVENTS_BlinkIntervalSet(interval, blinkInterval);
      ASSERT_PARAM_BLINK_INTERVAL_SIZE(interval+1);
      ASSERT_PARAM_BLINK_INTERVAL(interval, blinkInterval);
    }
    
  }

  // ----------------------------------------------------------------------
  // Handlers for typed from ports
  // ----------------------------------------------------------------------

  void LedTester ::
    from_gpioSet_handler(
        NATIVE_INT_TYPE portNum,
        const Fw::Logic& state
    )
  {
    // TODO
  }

}
