# ros-esp32-uart-communication
communication of ros with esp32 via uart and turtle_teleop_key application

## --- ROS ---
 * Let's run a node to start uart communication with esp on ubuntu and subscribe to / turtle1 / cmd_vel.
   - rosrun esp-idf-test serialT.py
 * then subscribe and read the keys on the keyboard will start the node.
   - rosrun turtlesim turtle_teleop_key 

## ---- ESP - IDF -------


 * This example shows how to use the UART driver to handle UART interrupt.
 
  - Port: UART0
  - Receive (Rx) buffer: on
  - Transmit (Tx) buffer: off
  - Flow control: off
  - Event queue: on
  - Pin assignment: TxD (default), RxD (default)
 
