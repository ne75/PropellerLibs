// I2C device class (I2Cdev) demonstration Processing sketch for MPU6050 DMP output
// 6/20/2012 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//     2012-06-20 - initial release

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2012 Jeff Rowberg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================
*/

import processing.serial.*;
import processing.opengl.*;
import toxi.geom.*;
import toxi.processing.*;

// NOTE: requires ToxicLibs to be installed in order to run properly.
// 1. Download from http://toxiclibs.org/downloads
// 2. Extract into [userdir]/Processing/libraries
//    (location may be different on Mac/Linux)
// 3. Run and bask in awesomeness

ToxiclibsSupport gfx;

int packetlength = 27;

int t = millis();

Serial port;                         // The serial port
char[] teapotPacket = new char[packetlength];  // InvenSense Teapot packet
char[] float_data = new char[packetlength-2];
int serialCount = 0;                 // current packet byte position
int fdata_cnt = 0;
int aligned = 0;
int interval = 0;

float[] q = new float[4];
Quaternion quat = new Quaternion(1, 0, 0, 0);

float[] gyro = new float[3];
float[] accel = new float[3];

void setup() {
    // 300px square viewport using OpenGL rendering
    //frameRate(90);
    size(600, 600, OPENGL);
    gfx = new ToxiclibsSupport(this);

    // setup lights and antialiasing
    lights();
    smooth();
  
    // display serial port list for debugging/clarity
    println(Serial.list());

    // get the first available port (use EITHER this OR the specific port code below)
    String portName = "/dev/tty.usbserial-DA00B3JW";
    
    // get a specific serial port (use EITHER this OR the first-available code above)
    //String portName = "COM4";
    
    // open the serial port
    port = new Serial(this, portName, 115200);
    
    // send single character to trigger DMP init/start
    // (expected by MPU6050_DMP6 example Arduino sketch)
    // port.write('r');
}

void draw() {    
    // black background
    background(50, 50, 50);
    
    // translate everything to the middle of the viewport
    pushMatrix();
    translate(width / 2, height / 2);


    // toxiclibs direct angle/axis rotation from quaternion (NO gimbal lock!)
    // (axis order [1, 3, 2] and inversion [-1, +1, +1] is a consequence of
    // different coordinate system orientation assumptions between Processing
    // and InvenSense DMP)
    
    stroke(255, 0, 0);
    strokeWeight(4);
    line(0, 0, 0, -accel[0]*100, accel[2]*100, accel[1]*100); 

    stroke(0, 255, 0);
    line(0, 0, 0, -gyro[0]*20, gyro[2]*20, gyro[1]*20); 
    
    Vec3D a = new Vec3D(accel[0], accel[1], accel[2]);
    Vec3D g = new Vec3D(gyro[0], gyro[1], gyro[2]);
    
    Quaternion a_exp_q = (quat.getConjugate().multiply(new Quaternion(0, 0, 0, 1))).multiply(quat);
    Vec3D a_exp = new Vec3D(a_exp_q.x, a_exp_q.y, a_exp_q.z);
    
    stroke(0, 255, 255);
    line(0, 0, 0, -a_exp.x*100, a_exp.z*100, a_exp.y*100); 
    
    Vec3D err = a.cross(a_exp);
    //err.z = 0;
    
    g = g.add(err);
    
    stroke(255, 128, 0);
    line(0, 0, 0, -err.x*20, err.z*20, err.y*20);
    
    int t0 = millis();
    int dt = t0 - t;
    t = t0;
    
    Quaternion qgyro = new Quaternion();
    qgyro.x = dt*g.x/2000.0;
    qgyro.y = dt*g.y/2000.0;
    qgyro.z = dt*g.z/2000.0;
    qgyro.w = 1.0 - (qgyro.x*qgyro.x + qgyro.y*qgyro.y + qgyro.z*qgyro.z)/2.0;
    
    Quaternion intq = qgyro.multiply(quat);
    if (intq.magnitude() - 1 < 0.1) { 
      quat = intq;
    }
    
    println(1000/dt);
    
    float[] axis = quat.normalize().toAxisAngle();
    Quaternion q = quat.normalize();
    rotate(axis[0], -axis[1], axis[3], axis[2]);
    
    stroke(0);
    strokeWeight(1);
    // draw main body in red
    fill(255, 0, 0, 200);
    //box(10, 200, 10);
    translate(0, -100, 0);
    drawCylinder(5, 5, 200, 4);
    
    // draw front-facing tip in blue
    fill(0, 128, 255, 200);
    pushMatrix();
    translate(0, -20, 0);
    //rotateX(PI/2);
    drawCylinder(0, 10, 20, 4);
    popMatrix();
    
    
    popMatrix();
}

void serialEvent(Serial port) {
    interval = millis();
    while (port.available() > 0) {
        int ch = port.read();
        if (ch == '$') {serialCount = 0;} // this will help with alignment
        if (aligned < 2) {
            if (serialCount == 0) {
                if (ch == '$') aligned++; else aligned = 0;
            } else if (serialCount == packetlength-1) {
                if (ch == '\n') aligned++; else aligned = 0;
            }
            //println(ch + " " + aligned + " " + serialCount);
            serialCount++;
            if (serialCount == packetlength) serialCount = 0;
        } else {
            if (serialCount > 0 || ch == '$') {
                teapotPacket[serialCount++] = (char)ch;
                if (serialCount == packetlength) {
                    serialCount = 0; // restart packet byte position

                      accel[0] = Float.intBitsToFloat((teapotPacket[4] << 24) | (teapotPacket[3] << 16) | (teapotPacket[2] << 8) | teapotPacket[1]);
                      accel[1] = Float.intBitsToFloat((teapotPacket[8] << 24) | (teapotPacket[7] << 16) | (teapotPacket[6] << 8) | teapotPacket[5]);
                      accel[2] = Float.intBitsToFloat((teapotPacket[12] << 24) | (teapotPacket[11] << 16) | (teapotPacket[10] << 8) | teapotPacket[9]);
                      
                      gyro[0] = Float.intBitsToFloat((teapotPacket[16] << 24) | (teapotPacket[15] << 16) | (teapotPacket[14] << 8) | teapotPacket[13]);
                      gyro[1] = Float.intBitsToFloat((teapotPacket[20] << 24) | (teapotPacket[19] << 16) | (teapotPacket[18] << 8) | teapotPacket[17]);
                      gyro[2] = Float.intBitsToFloat((teapotPacket[24] << 24) | (teapotPacket[23] << 16) | (teapotPacket[22] << 8) | teapotPacket[21]);
                   

//                    // get quaternion from data packet, converting fixed point 32 bit numbers to floats. 
//                    q[0] = Float.intBitsToFloat((teapotPacket[4] << 24) | (teapotPacket[3] << 16) | (teapotPacket[2] << 8) | teapotPacket[1]);
//                    q[1] = Float.intBitsToFloat((teapotPacket[8] << 24) | (teapotPacket[7] << 16) | (teapotPacket[6] << 8) | teapotPacket[5]);
//                    q[2] = Float.intBitsToFloat((teapotPacket[12] << 24) | (teapotPacket[11] << 16) | (teapotPacket[10] << 8) | teapotPacket[9]);
//                    q[3] = Float.intBitsToFloat((teapotPacket[16] << 24) | (teapotPacket[15] << 16) | (teapotPacket[14] << 8) | teapotPacket[13]);
//                    // for (int i = 0; i < 4; i++) if (q[i] >= 2) q[i] = -4 + q[i];
//                  
//                    // set our toxilibs quaternion to new data
//                    if (sqrt(q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3]) - 1 < 0.05) { 
//                      quat.set(q[0], q[1], q[2], q[3]);
//                    }
                    
        
                    // output various components for debugging
                    // println("q: " + sqrt(q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3]) + "\t" + round(q[0]*100.0f)/100.0f + "\t" + round(q[1]*100.0f)/100.0f + "\t" + round(q[2]*100.0f)/100.0f + "\t" + round(q[3]*100.0f)/100.0f);
                    //println("euler:\t" + euler[0]*180.0f/PI + "\t" + euler[1]*180.0f/PI + "\t" + euler[2]*180.0f/PI);
                    //println("ypr:\t" + ypr[0]*180.0f/PI + "\t" + ypr[1]*180.0f/PI + "\t" + ypr[2]*180.0f/PI);
                    
                }
            }
        }
    }
}

void drawCylinder(float topRadius, float bottomRadius, float tall, int sides) {
    float angle = 0;
    float angleIncrement = TWO_PI / sides;
    beginShape(QUAD_STRIP);
    for (int i = 0; i < sides + 1; ++i) {
        vertex(topRadius*cos(angle), 0, topRadius*sin(angle));
        vertex(bottomRadius*cos(angle), tall, bottomRadius*sin(angle));
        angle += angleIncrement;
    }
    endShape();
    
    // If it is not a cone, draw the circular top cap
    if (topRadius != 0) {
        angle = 0;
        beginShape(TRIANGLE_FAN);
        
        // Center point
        vertex(0, 0, 0);
        for (int i = 0; i < sides + 1; i++) {
            vertex(topRadius * cos(angle), 0, topRadius * sin(angle));
            angle += angleIncrement;
        }
        endShape();
    }
  
    // If it is not a cone, draw the circular bottom cap
    if (bottomRadius != 0) {
        angle = 0;
        beginShape(TRIANGLE_FAN);
    
        // Center point
        vertex(0, tall, 0);
        for (int i = 0; i < sides + 1; i++) {
            vertex(bottomRadius * cos(angle), tall, bottomRadius * sin(angle));
            angle += angleIncrement;
        }
        endShape();
    }
}
