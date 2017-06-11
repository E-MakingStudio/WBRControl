#include "WBRCtrl.h"




// �s���ԍ��A�T�C��(0�̓Z���T�[���g�p���Ȃ����Ƃ��w��j
#define LEFT_MOTOR_PWM_GO       11    // �����[�^�[�̂o�v�l���ڏo�̓s��(�f�W�^��)
#define LEFT_MOTOR_PWM_BACK     10    // �����[�^�[�̂o�v�l�t�ڏo�̓s��(�f�W�^��)
#define RIGHT_MOTOR_PWM_GO      6     // �E���[�^�[�̂o�v�l���ڏo�̓s��(�f�W�^��)
#define RIGHT_MOTOR_PWM_BACK    9     // �E���[�^�[�̂o�v�l�t�ڏo�̓s��(�f�W�^��)
#define READ_BATTERY            8     //�@�d������p�s��(�Z���T�ԍ���)
#define FRONT_RIGHT_SENSOR      3     // �O���E�Z���T�̃s���ԍ�(�f�W�_��)
#define FRONT_LEFT_SENSOR       5     // �O�����Z���T�̃s���ԍ��i�f�W�^���j
#define FRONT_CENTER_SENSOR     0     // �O�������Z���T�̃s���ԍ��i���݂��Ȃ��ꍇ��0��ݒ�j�i�f�W�^���j
#define SIDE_RIGHT_SENSOR       2     // �E���ʃZ���T�̃s���ԍ��i�f�W�^���j
#define SIDE_LEFT_SENSOR        7     // �����ʃZ���T�̃s���ԍ��i�f�W�^���j
#define MAGNET_SENSOR           12    // ���d���Ŏ��͂����m���邽�߂̃Z���T�̃s���ԍ�(���������Z���T�����݂���̂��낤���H)(�Z���T�ԍ���)
#define RIGHT_ROTARY_ENCODER    15    // �E���[�^���[�G���R�[�_�̃Z���T�̃s���ԍ�
#define LEFT_ROTARY_ENCODER     14    // �����[�^���[�G���R�[�_�̃Z���T�̃s���ԍ�

// �s���ԍ�
int rightMotorPWMGo = RIGHT_MOTOR_PWM_GO;
int rightMotorPWMBack = RIGHT_MOTOR_PWM_BACK;
int leftMotorPWMGo = LEFT_MOTOR_PWM_GO;
int leftMotorPWMBack = LEFT_MOTOR_PWM_BACK;
int readBattery = READ_BATTERY;
int frontRightSensor = FRONT_RIGHT_SENSOR;
int frontLeftSensor = FRONT_LEFT_SENSOR;
int frontCenterSensor = FRONT_CENTER_SENSOR;
int sideRightSensor = SIDE_RIGHT_SENSOR;
int sideLeftSensor = SIDE_LEFT_SENSOR;
int magnetSensor = MAGNET_SENSOR;
int rightRotary_Encoder = RIGHT_ROTARY_ENCODER;
int leftRotary_Encoder = LEFT_ROTARY_ENCODER;

WBRCtrl wbr(
	rightMotorPWMGo,
	rightMotorPWMBack,
	leftMotorPWMGo,
	leftMotorPWMBack,
	readBattery,
	frontRightSensor,
	frontLeftSensor,
	frontCenterSensor,
	sideRightSensor,
	sideLeftSensor,
	magnetSensor,
	rightRotary_Encoder,
	leftRotary_Encoder
);

void setup()
{

	wbr.PinInitialization();
	Serial.begin(9600);

}

void loop()
{
	wbr.main();
  /* add main program code here */

}

