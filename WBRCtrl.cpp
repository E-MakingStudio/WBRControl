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
  Direction direct = Left;

  if (CheckBattery(START_BATTERY))//起動電圧に達していないことを確認
  {
    delay(CHARGE_DELAY_TIME);//受電
  }

  while (CheckBattery(BAD_BATTERY))//起動最低電圧でないかを確認
  {
    SetPwmMove();
    
    while (IsEdge() == true)
    {
      if (IsCorner() == true)
      {
        Turn(deg180, direct);

        direct = direct == Right ? Left : Right;//次の角の回転方向を決定する。

        break;//continueでもいいけど気分で"Break"
      }
      FloorDirectionTurning();//モーターのPWMが停止状態で帰ってくるので注意
    }

    //帰還用の関数を書く必要があります。
      
  }
    
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
  int Edge = false;

  int FRcensor = digitalRead(PinFRSensor);
  int FLcensor = digitalRead(PinFLSensor);
  int FCcensor = digitalRead(PinFCSensor);

  FCcensor = (PinFCSensor == 0) ? 0 : FCcensor;

  if (FRcensor == FLOOR_NOTEXIST || FLcensor == FLOOR_NOTEXIST || FCcensor == FLOOR_NOTEXIST)//どれか一つにセンサーに引っかかったら
  {
    if (FRcensor == FLOOR_EXIST || FLcensor == FLOOR_EXIST)
    {
      //FloorDirectionTurning();
      return Edge = true;
    }
  }

  return Edge;

}

/// <summary>
/// 角かどうかを確認する関数
/// </summary>
/// <param name="direct">回転方向</param>
/// <returns>角であれば[true]でなければ[false]</returns>
bool WBRCtrl::IsCorner()
{
  int SideRcensor = digitalRead(PinSideRSensor);
  int SideLcensor = digitalRead(PinSideLSensor);

  if (SideRcensor == FLOOR_EXIST || SideLcensor == FLOOR_EXIST)
  {
    return true;
  }

  return false;
}


///<summary>バッテリーの残量確認</summary>
///<param name='batteryTartgetValue'>バッテリー電圧値 </param>
///<return>バッテリー電圧が所定の値を満たしているかどうか</return>
///
bool WBRCtrl::CheckBattery(int batteryTartgetValue)
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
      SetPwmTurn(Left);
    }
  }

  if (Lcensor == FLOOR_EXIST)
  {
    while (digitalRead(PinFRSensor) != digitalRead(PinFLSensor))
    {
      SetPwmTurn(Right);
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
  if (dic == Right)
  {
    analogWrite(PinLmotorPWMf, TURN_PWM);
    analogWrite(PinLmotorPWMb, 0);
    analogWrite(PinRmotorPWMf, 0);
    analogWrite(PinRmotorPWMb, TURN_PWM);
    return;
  }
  analogWrite(PinLmotorPWMf, 0);
  analogWrite(PinLmotorPWMb, TURN_PWM);
  analogWrite(PinRmotorPWMf, TURN_PWM);
  analogWrite(PinRmotorPWMb, 0);

  return;
}

/// <summary>
/// モーターの回転数を前進状態に設定する
/// </summary>
void WBRCtrl::SetPwmMove()
{
  analogWrite(PinLmotorPWMf, RIGHT_PWM_SPEED);
  analogWrite(PinLmotorPWMb, 0);

 

  analogWrite(PinRmotorPWMf, LEFT_PWM_SPEED);
  analogWrite(PinRmotorPWMb, 0);



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

/// <summary>
/// 回転を行う関数
/// </summary>
/// <param name="angle">角度[90deg,180deg]</param>
/// <param name="direct">方向[Right,Left]</param>
void WBRCtrl::Turn(Angle angle, Direction direct)
{

  if (angle == deg90)
  {
    for (int i = 0; i < 5; i++)	//5は仮の値
    {
      SetPwmTurn(direct);

      delay(SPIN_DELAY_TIME);
    }
    return;
  }

  Turn(deg90, direct);

  SetPwmMove();
  StopRollByCount(1, 1);
  SetPwmStop();

  Turn(deg90, direct);

  if (direct == NEXT_TURN_LEFT)
  {
    direct = (Direction)NEXT_TURN_RIGHT;
  }

  if (direct == NEXT_TURN_RIGHT)
  {
    direct = (Direction)NEXT_TURN_LEFT;
  }
  return;

}

void WBRCtrl::BackHome()
{
  int magnet_sencer = 1;


}

/// <summary>
/// 指定の回転数回転を行わせる関数
/// </summary>
/// <param name="RspinCount_TargetCount">右モーターの指定回転数</param>
/// <param name="LspinCount_TargetCount">左モーターの指定回転数</param>
void WBRCtrl::StopRollByCount(int RspinCount_TargetCount, int LspinCount_TargetCount)
{
  RspinCount_TargetCount, LspinCount_TargetCount *= SPINCOUNT_TARGETVALUE;//一回転あたりのパルス数と回転数をかけて必要なパルス数を算出

  for (int RspinCount, LspinCount = 0; RspinCount_TargetCount >= RspinCount && LspinCount_TargetCount >= LspinCount;)//規定のパルス数パルスが発振されるまで繰り返す
  {
    if (digitalRead(PinRRotary_Encoder) == 1)
    {
      RspinCount++;
    }
    if (digitalRead(PinLRotary_Encoder) == 1)
    {
      LspinCount++;
    }
  }
}

/// <summary>
/// 角までの距離(タイヤの回転回数)を計測する関数
/// </summary>
/// <returns>タイヤの回転した回数</returns>
int WBRCtrl::MeasureDistanceByRoll()
{
  SetPwmMove();
  int spinCount = 0;
  for (int RspinCount, LspinCount = 0;;)
  {
    if (IsCorner())
    {
      SetPwmStop();
      spinCount = RspinCount < LspinCount ? RspinCount : LspinCount;
      break;
    }
    if (digitalRead(PinRRotary_Encoder) == 1)
    {
      RspinCount++;
    }
    if (digitalRead(PinLRotary_Encoder) == 1)
    {
      LspinCount++;
    }
  }
  return spinCount / SPINCOUNT_TARGETVALUE;

}
