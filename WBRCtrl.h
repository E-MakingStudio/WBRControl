#pragma once

#ifndef WBRCTRL_H_
#define WBRCTRL_H_
// WBRMain
#define BAD_BATTERY			0.5		//	�o�b�e���[�������Ȃ����̓d��
//�@FloorCheck
// (���Ɏc�O�Ȃ��ƂɁA��������Ƃ��ɃZ���T���Ԃ��l�͂O[LOW]�ł�)
#define FLOOR_EXIST			0		// ������
#define FLOOR_NOTEXIST		1		// ���Ȃ�
//�@CornerCheck
#define CORNER_EXIST		1		// �p����
#define CORNER_NOTEXIST		0		// �p�Ȃ�
//�@FloorDirectionturning
#define TURN_PWM				145		// ���[�^�[�̉�]��
#define FLOOR_DELAY_TIME 		100		// �������̑ҋ@����
//�@GoForward
#define RIGHT_PWM_SPEED		179		// �O�i���E���[�^�[�̉�]��
#define LEFT_PWM_SPEED		234		// �O�i�������[�^�[�̉�]��
//�@StartBatteryCheck
#define START_BATTERY			3.5		//�\���Ƀo�b�e���[���[�d����Ă���Ƃ��̓d��
#define CHARGE_DELAY_TIME		1000		// �[�d�ҋ@����\
// Turn90deg
#define SPIN_DELAY_TIME		500		// ��]���̑ҋ@����
#define NEXT_TURN_RIGHT		0		//��]�������E����
#define NEXT_TURN_LEFT		1		//��]������������
//�@BackHome
#define MGNET_SERACH_DELAY_TIME		500		// ���͊��m�̂��߂̑ҋ@����
//�@Rotary_Encoder
#define ROTARY_ENCODER_SPIN		1		//�^�C���̉�]�����w��
#define SPINCOUNT_TARGETVALUE		4		//���]�ɕK�v�ȃp���X��
//	
#define SPINCOUNT_BODY_LENGTH	  114514			//��{�̕��ړ�����ׂ̃p���X��


#include <arduino.h>
/**
�E�A���̕����̗񋓌^
**/
enum Direction
{
	Right = 0, Left = 1
};
/**
��]�̊p�x�̗񋓌^
**/
enum Angle
{
	deg90, deg180
};

class WBRCtrl
{
private:
	int PinRmotorPWMf;	//�E���[�^�[��PWM���ڏo�̓s��(2)
	int	PinLmotorPWMf;	//�����[�^�[��PWM���ڏo�̓s��(2)
	int PinRmotorPWMb;	//�E���[�^�[��PWM�t�ڏo�̓s��(2)
	int PinLmotorPWMb;	//�����[�^�[��PWM�t�ڏo�̓s��(2)
	int PinReadBattery;
	int	PinFRSensor;
	int	PinFLSensor;
	int	PinFCSensor;
	int	PinSideRSensor;
	int	PinSideLSensor;
	int	PinMagnetSensor;
	int	PinRRotary_Encoder;
	int	PinLRotary_Encoder;

public:
	WBRCtrl
	(
		int PinRmotorPWMf,
		int	PinLmotorPWMf,
		int PinRmotorPWMb,
		int PinLmotorPWMb,
		int PinReadBattery,
		int	PinFRSensor,
		int	PinFLSensor,
		int	PinFCSensor,
		int	PinSideRSensor,
		int	PinSideLSensor,
		int	PinMagnetSensor,
		int	PinRRotary_Encoder,
		int	PinLRotary_Encoder
	);

	int	main();
	void PinInitialization();
	int FloorCheck();
	int CornerCheck(Direction *direct);
	bool IsBatteryFully(bool IsStart);
	void FloorDirectionTurning();
	void FloorTurning(Direction dic);
	void move();
	void stop();
	void Turn(Angle angle,Direction *direct);
	void BackHome(Direction *direct);
	void Rotary_Encoder(int RSpinCount_TargetValue, int LSpinCount_TargetValue);

};


#endif

