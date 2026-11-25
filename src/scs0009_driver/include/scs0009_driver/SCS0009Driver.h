
#ifndef SCS0009_DRIVER_H
#define SCS0009_DRIVER_H

#include <string>
#include <vector>

#include "SCSCL.h"

class SCS0009Driver
{
public:
    struct Feedback
    {
        int position;
        int speed;
        int load;
        int voltage;
        int temperature;
        int moving;
        int current;
    };

    SCS0009Driver();

    // Serial connection
    bool connect(const std::string& port, int baudrate);
    void disconnect();
    bool isConnected() const;

    // Servo communication
    bool ping(int servo_id);

    // Torque control
    bool enableTorque(int servo_id);
    bool disableTorque(int servo_id);

    // Position control
    bool setPosition(
        int servo_id,
        int position,
        int time,
        int speed
    );

    // Registered position control
    bool registerPosition(
        int servo_id,
        int position,
        int time,
        int speed
    );

    bool executeRegisteredCommands();

    // Coordinated multi-servo position control
    // Empty time/speed vectors mean zero for those parameters.
    // True means the SDK call was issued, not that every servo acknowledged it.
    bool syncWritePosition(
        const std::vector<int>& servo_ids,
        const std::vector<int>& positions,
        const std::vector<int>& times,
        const std::vector<int>& speeds
    );

    // Feedback
    bool readFeedback(int servo_id, Feedback& feedback);

private:
    bool isValidServoId(int servo_id) const;

    SCSCL servo_;
    bool connected_;
};

#endif
