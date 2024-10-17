#include "M5Capsule.h"
#include <Arduino.h>
#include <accIntegral.h>

accIntegral fusion;
// Filter coefficients                       //  Unit           
constexpr float GRAVITY = 9.81e3;            //  mm/s^2             Magnitude of gravity at rest. Determines units of velocity. [UNITS MUST MATCH ACCELERATION]
constexpr float SD_ACC  = 1000 / GRAVITY;    //  mm/s^2 / g-force   Standard deviation of acceleration. Deviations from zero are suppressed.
constexpr float SD_VEL  = 200  / GRAVITY;    //  mm/s   / g-force   Standard deviation of velocity. Deviations from target value are suppressed.
constexpr float ALPHA   = 0.5;               //                     Gain of heading update - See example "output" for more information.
constexpr float topai = 0.017444;

void setup() 
{    
    // initialize M5
    auto cfg = M5.config();
    M5.begin(cfg);
    Serial.begin(115200); // arduino must set to display
    
    // initialize IMU Filter
    auto imu_update = M5.Imu.update();
    if (imu_update)
    {
        // get data from IMU
        auto capture_data = M5.Imu.getImuData();
        fusion.setup( capture_data.accel.x, capture_data.accel.y, capture_data.accel.z );   // ALWAYS set initial heading with acceleration 
    }
    else
    {
        fusion.setup();
    }
}

void loop() 
{
    M5.update();
  
    // update IMU data
    auto imu_update = M5.Imu.update();
    if (imu_update)
    {
        // get data from IMU
        auto capture_data = M5.Imu.getImuData();

        // try reset fusion
        if (M5.BtnA.wasPressed())
        {
            M5.Speaker.tone(10000, 20);
            fusion.setup( capture_data.accel.x, capture_data.accel.y, capture_data.accel.z );
        }

        // get velocity
        vec3_t vel;
        {
            vec3_t accel = { capture_data.accel.x, capture_data.accel.y, capture_data.accel.z };    // g-unit
            vec3_t gyro = { capture_data.gyro.x, capture_data.gyro.y, capture_data.gyro.z };     // radians/second
            gyro = gyro * topai;
            // Update heading and velocity estimate:

            // known measured velocity (target state). Estimate will be forced towards this vector
            vec3_t vel_t = {0,0,0};

            vel_t /= GRAVITY;                         // must have unit: g-force * second
            
            /* note: all coefficients are optional and have default values */
            fusion.update( gyro, accel, vel_t, SD_ACC, SD_VEL, ALPHA ); 

            // obtain velocity estimate
            vel = fusion.getVel() * GRAVITY;   // scale by gravity for desired units
            
            if(vel.mag()>10.0)
            {
                // Display velocity components: [view with serial plotter]
                Serial.print( vel.x, 2 );
                Serial.print( " " );
                Serial.print( vel.y, 2 );
                Serial.print( " " );
                Serial.print( vel.z, 2 );  
                Serial.println();
            }
        }
    }
}
