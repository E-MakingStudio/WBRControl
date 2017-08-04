#include "WBRCtrl.h"


//
WBRCtrl::WBRCtrl
(
	int _PinRmotorPWMf,
	int	_PinLmotorPWMf,
	int _PinRmotorPWMb,
	int _PinLmotorPWMb,
	int _PinReadBattery,
	int	_PinFRSensor,
	int	_PinFLSensor,
	int	_PinFCSensor,
	int	_PinSideRSensor,
	int	_PinSideLSensor,
	int	_PinMagnetSensor,
	int	_PinRRotary_Encoder,
	int	_PinLRotary_Encoder

)
{
	PinRmotorPWMf = _PinRmotorPWMf;
	PinLmotorPWMf = _PinLmotorPWMf;
	PinRmotorPWMb = _PinRmotorPWMb;
	PinLmotorPWMb = _PinLmotorPWMb;
	PinReadBattery = _PinReadBattery;
	PinFRSensor = _PinFRSensor;
	PinFLSensor = _PinFLSensor;
	PinFCSensor = _PinFCSensor;
	PinSideRSensor = _PinSideRSensor;
	PinSideLSensor = _PinSideLSensor;
	PinMagnetSensor = _PinMagnetSensor;
	PinRRotary_Encoder = _PinRRotary_Encoder;
	PinLRotary_Encoder = _PinLRotary_Encoder;


}
void WBRCtrl::PinInitialization()
{
	pinMode(PinRmotorPWMf, OUTPUT);
	pinMode(PinLmotorPWMf, OUTPUT);
	pinMode(PinRmotorPWMb, OUTPUT);
	pinMode(PinLmotorPWMb, OUTPUT);
	pinMode(PinReadBattery, INPUT);
	pinMode(PinFLSensor, INPUT);
	pinMode(PinFCSensor, INPUT);
	pinMode(PinFRSensor, INPUT);
	pinMode(PinSideRSensor, INPUT);
	pinMode(PinSideLSensor, INPUT);
	pinMode(PinMagnetSensor, INPUT);
	pinMode(PinRRotary_Encoder, INPUT);
	pinMode(PinLRotary_Encoder, INPUT);
}

/// <summary>
/// メイン関数
/// </summary>
/// <returns></returns>
int WBRCtrl::main()
{
	int ConerCount=0;
	Direction direct = Left;
  Serial.println("main＿in");
/*	while (CheckBattery(START_BATTERY))//起動電圧に達していないことを確認
	{
		delay(CHARGE_DELAY_TIME);//受電
	}
	 
 */
	while (1)//起動最低電圧でないかを確認
	{
		SetPwmMove(Forward, Forward);

		if (IsEdge() == true)
		{
     
			TurnDeg180(direct);
			direct = direct == Right ? Left : Right;//次の角の回転方向を決定する。
			
			if (IsCorner() == true)
			{	
        
        ConerCount++;
				if(ConerCount >= 2){
				//2はdefainで指定してね		
        		
				break;
				}
			}			
		}
	}
	
	SetPwmStop();
  Serial.println("main＿out");
		//帰還用関数が必要

	/*以下計測とそれに応じた挙動を設定するコード　論理的に破綻したためパージ
	  //計測開始
	  int width = MeasureDistanceByRoll();// / SPINCOUNT_BODY_LENGTH;//横方向の計測値/本体一体分の回転回数
	  Turn(deg90, Left);
	  int	height = MeasureDistanceByRoll()/ SPINCOUNT_BODY_LENGTH;//縦方向の計測値/本体一体分の回転回数
	  Turn(deg90, Left);
	  x_location = 0;
	  y_location = height;

	  while (y_location <= SPINCOUNT_BODY_LENGTH);
	  {
	  Move();
	  RolltoStopByCount(width,width);
	  Stop();
	  Turn(deg180, Left);
	  }

	*/
}



/// <summary>
/// ルンバがホワイトボードの端に来たことを検知
/// </summary>
/// <returns>端かどうか</returns>
bool WBRCtrl::IsEdge()
{
  Serial.println("IsEdge_in");
	int Edge = false;

	int FRcensor = digitalRead(PinFRSensor);
	int FLcensor = digitalRead(PinFLSensor);
	int FCcensor = digitalRead(PinFCSensor);

	FCcensor = (PinFCSensor == 0) ? 0 : FCcensor;

	if (FRcensor == FLOOR_NOTEXIST || FLcensor == FLOOR_NOTEXIST || FCcensor == FLOOR_NOTEXIST)//どれか一つにセンサーに引っかかったら
	{
		FloorDirectionTurning();
		Serial.println("IsEdge_out,True");
		return Edge = true;
	}
  Serial.println("IsEdge_out,False");
	return Edge;

}

/// <summary>
/// 角かどうかを確認する関数
/// </summary>
/// <param name="direct">回転方向</param>
/// <returns>角であれば[true]でなければ[false]</returns>
bool WBRCtrl::IsCorner()
{
  Serial.println("InCorner_in");
	int SideRcensor = digitalRead(PinSideRSensor);
	int SideLcensor = digitalRead(PinSideLSensor);

	if (SideRcensor == FLOOR_EXIST || SideLcensor == FLOOR_EXIST)
	{
    Serial.println("InCorner_out,True");
		return true;
	}
 
  Serial.println("InCorner_out,False");
	return false;
}


///<summary>バッテリーの残量確認</summary>
///<param name='batteryTartgetValue'>バッテリー電圧値 </param>
///<return>バッテリー電圧が所定の値を満たしているかどうか</return>
///
bool WBRCtrl::CheckBattery(double batteryTartgetValue)
{
  return analogRead(PinReadBattery) <= batteryTartgetValue ? false : true;
}
/// <summary>
/// ホワイトボード上下左右の、橋からルンバが飛び出そうとしたときに回転させてもとに戻る関数
/// </summary>
void WBRCtrl::FloorDirectionTurning()
{
	int Rcensor = digitalRead(PinFRSensor);
	int Lcensor = digitalRead(PinFLSensor);

	if (Rcensor == FLOOR_EXIST)
	{

		while (digitalRead(PinFRSensor) != digitalRead(PinFLSensor))
		{
			SetPwmTurn(Right);
		}
	}

	else if (Lcensor == FLOOR_EXIST)
	{
		while (digitalRead(PinFRSensor) != digitalRead(PinFLSensor))
		{
			SetPwmTurn(Left);
		}
	}

	SetPwmStop();

	return;

}
/*
  床方向調整用

*/
void WBRCtrl::SetPwmTurn(Direction dic)
{
  Serial.println("SetPwmTurn_in");
	if (dic == Right)
	{
		analogWrite(PinLmotorPWMf, TURN_PWM);
		analogWrite(PinLmotorPWMb, 0);
		analogWrite(PinRmotorPWMf, 0);
		analogWrite(PinRmotorPWMb, TURN_PWM);
		return;
	}
 else if(dic == Left)
 {
	analogWrite(PinLmotorPWMf, 0);
	analogWrite(PinLmotorPWMb, TURN_PWM);
	analogWrite(PinRmotorPWMf, TURN_PWM);
	analogWrite(PinRmotorPWMb, 0);
  Serial.println("SetPwmTurn_out");
	return;
 }
}
/// <summary>
/// 
/// </summary>
/// <param name="Rdic"></param>
/// <param name="Ldic"></param>
void WBRCtrl::SetPwmMove(Direction Rdic, Direction Ldic)
{

	analogWrite(PinLmotorPWMf, Rdic == Forward ? RIGHT_PWM_SPEED : 0);
	analogWrite(PinLmotorPWMb, Rdic == Back ? RIGHT_PWM_SPEED : 0);

	analogWrite(PinRmotorPWMf, Ldic == Forward ? LEFT_PWM_SPEED : 0);
	analogWrite(PinRmotorPWMb, Ldic == Back ? LEFT_PWM_SPEED : 0);

	return;
}


/// <summary>
/// モーターの回転を停止状態に設定する
/// </summary>
void WBRCtrl::SetPwmStop()
{
	analogWrite(PinLmotorPWMb, 0);
	analogWrite(PinLmotorPWMf, 0);

	analogWrite(PinRmotorPWMb, 0);
	analogWrite(PinRmotorPWMf, 0);

	return;
}

void WBRCtrl::TurnDeg180(Direction dic){

	Serial.println("In Turn180deg.");

		// 回転の際にボードのフチに干渉しないように少し後方へ
	SetPwmMove(Back,Back);
		
	MoveByCount(2,2);//計算は後で

	SetPwmStop();
	
		// その場で90度回転
	SetPwmTurn(dic);
	MoveByCount(4, 4);
	
		//車体分前進
	SetPwmMove(Forward,Forward);
		
	MoveByCount(3, 3);

	SetPwmStop();

		// もう一度その場で90度回転
	SetPwmTurn(dic);
	MoveByCount(4, 4);
	Serial.println("Out Turn180deg.");
}


/// <summary>
/// 指定の回転数回転を行わせる関数
/// </summary>
/// <param name="RspinCount_TargetCount">右モーターの指定回転数</param>
/// <param name="LspinCount_TargetCount">左モーターの指定回転数</param>
/*
*左右のタイヤについているロータリーエンコーダのパルスを見て、タイヤが指定された回数回転するまで監視する
 *回転量の制御には「x回転」のxを引数に入力してください。
 *センサが羽を通過しているときは出力が0[LOW]、通過していないときは1[HI]が帰ってきます。
 *車輪のサイズは17/6/16時点では56mm,車体の円の直径は180mmです.
 */
void WBRCtrl::MoveByCount(float RspinCount_TargetCount, float LspinCount_TargetCount)
{
	//一回転に必要なパルス数にタイヤの回転数をかけて、必要なパルス数を用意する
	//(ここで、呼びされたときに得た引数を用いて必要な回転量分のパルス数を計算)
	RspinCount_TargetCount, LspinCount_TargetCount *= SPINCOUNT_TARGETVALUE;

	// forループ内で前回のループのセンサの状態を保存する変数を定義
	int bf_RRotary_Encoder = 0;
	int bf_LRotary_Encoder = 0;

	for (int RspinCount, LspinCount = 0; RspinCount_TargetCount >= RspinCount && LspinCount_TargetCount >= LspinCount;)//規定のパルス数パルスが発振されるまで繰り返す
	{
		int statRR_Encoder, statLR_Encoder;

		statRR_Encoder = digitalRead(PinRRotary_Encoder);
		statLR_Encoder = digitalRead(PinLRotary_Encoder);


		if (statRR_Encoder == 0)
		{
			if (bf_RRotary_Encoder == 1) 
			{

				RspinCount++;
			}
		}

		if (statLR_Encoder == 0)
		{
			if (bf_LRotary_Encoder == 1) 
			{

				LspinCount++;
			}
		}
		// 今回のセンサの状態を次のループへ持ち越す
		bf_RRotary_Encoder = statRR_Encoder;
		bf_LRotary_Encoder = statLR_Encoder;
	}
}

/// <summary>
/// 角までの距離(タイヤの回転回数)を計測する関数
/// </summary>
/// <returns>タイヤの回転した回数</returns>
int WBRCtrl::MeasureDistanceByRoll()
{
	SetPwmMove(Forward, Forward);
	int spinCount = 0;

	int bf_RRotary_Encoder = 0;
	int bf_LRotary_Encoder = 0;
	for (int RspinCount, LspinCount = 0;;)
	{

		int statRR_Encoder, statLR_Encoder;

		statRR_Encoder = digitalRead(PinRRotary_Encoder);
		statLR_Encoder = digitalRead(PinLRotary_Encoder);
		
		if (IsCorner())
		{

			SetPwmStop();
			spinCount = RspinCount;
			break;
		}

		if (statRR_Encoder == 0)
		{
			if (bf_RRotary_Encoder == 1) 
			{
				
				RspinCount++;
			}
		}

		if (statLR_Encoder == 0)
		{
			if (bf_LRotary_Encoder == 1)
			{

				LspinCount++;
			}
		}
		// 今回のセンサの状態を次のループへ持ち越す
		bf_RRotary_Encoder = statRR_Encoder;
		bf_LRotary_Encoder = statLR_Encoder;
	}


	return spinCount / SPINCOUNT_TARGETVALUE;

}

