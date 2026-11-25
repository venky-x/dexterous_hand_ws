#include <iostream>
#include "scs0009_driver/SCS0009Driver.h"

int main()
{
    const std::string port = "/dev/ttyACM_servo";
    const int baudrate = 1000000;
    const int servo_id = 1;

    SCS0009Driver driver;

    std::cout << "Connecting to SCS0009..." << std::endl;

    if (!driver.connect(port, baudrate))
    {
        std::cerr << "ERROR: Connection failed." << std::endl;
        return 1;
    }

    std::cout << "Connected successfully." << std::endl;

    if (!driver.ping(servo_id))
    {
        std::cerr << "ERROR: Servo did not respond." << std::endl;
        driver.disconnect();
        return 1;
    }

    std::cout << "Servo ID " << servo_id << " responded." << std::endl;

    SCS0009Driver::Feedback feedback;

    if (!driver.readFeedback(servo_id, feedback))
    {
        std::cerr << "ERROR: Could not read position." << std::endl;
        driver.disconnect();
        return 1;
    }

    std::cout << "\n===== SERVO FEEDBACK =====" << std::endl;
    std::cout << "Position:    " << feedback.position << std::endl;
    std::cout << "Speed:       " << feedback.speed << std::endl;
    std::cout << "Load:        " << feedback.load << std::endl;
    std::cout << "Voltage:     " << feedback.voltage << std::endl;
    std::cout << "Temperature: " << feedback.temperature << std::endl;
    std::cout << "Current:     " << feedback.current << std::endl;

    driver.disconnect();

    std::cout << "\nDisconnected successfully." << std::endl;

    return 0;
}
