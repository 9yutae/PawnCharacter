# 🎮 PawnCharacter

## 📌 프로젝트 개요
본 프로젝트는 팀스파르타코딩클럽 내일배움캠프의 'Unreal 기반 3D 게임 개발자 양성과정' 2기   
Ch03의 7번 과제에 대한 소스 코드입니다.   
- 프로젝트 기간 : 2025.01.20 ~ 2025.02.05
  
</br>

## 🎯 필수 기능
✅ 1. C++ Pawn 클래스 구현 (이동 시스템)
   - CapsuleComponent, SkeletalMeshComponent, SpringArm, CameraComponent를 Pawn에 추가
   - `GameMode`에서 `DefaultPawnClass`를 지정하여 Pawn이 컨트롤 가능하도록 설정

✅ 2. Enhanced Input을 활용한 입력 처리
   - Input 액션 생성 및 매핑 (WASD 이동, 마우스 회전)
   - `SetupPlayerInputComponent()`에서 Enhanced Input 액션을 바인딩
   - `AddActorLocalOffset()`, `AddActorLocalRotation()`을 활용하여 직접 이동 및 회전 로직 구현
     
</br>

## 🏆 도전 과제
🔥 1. 6자유도 (6DOF) 드론/비행체 이동 시스템
   - WASD + Space/Shift를 활용한 X/Y/Z 축 이동
   - 마우스 및 키보드 입력을 활용하여 Yaw/Pitch/Roll 회전
   - 현재 회전 상태를 고려한 방향 이동 구현

🔥 2. 중력 및 낙하 시스템
   - `Tick()`에서 직접 중력 가속도를 계산하여 중력 적용
   - `Sweep` 활용하여 바닥 감지 및 착지 처리
   - 공중과 지상에서 다른 이동 속도를 적용하여 자연스러운 이동 구현
     
</br>

## 🛠 추가로 구현한 기능
  - 점프(Spacebar), 달리기(Shift) 액션 및 액터, 카메라 회전(마우스 우클릭) 로직 추가
  - 애니메이션 적용
  - 보간(Interpolation) 적용
    
</br>

## 📹 과제 제출 영상
[NBC Assignment 07 Pawn](https://youtu.be/3dHEaC0ugvk)

</br>

## 📦 에셋 출처
Level : https://www.fab.com/listings/b4e88a64-2388-4c46-bf0a-b48be046f722

UAV : https://www.fab.com/listings/5b7e653f-07fe-4131-9d5d-8fe15b1fd0fc

</br>

## 📂 프로젝트 구조
```Text
/Source
    /PawnCharacter
        /Actors
            NBC_Pawn.h / .cpp              // Pawn 클래스
            NBC_GameMode.h / .cpp
            NBC_PlayerController.h / .cpp
            NBC_AnimInstance.h / .cpp
            NBC_UAV.h / .cpp               // 드론(UAV) 클래스
            NBC_UAVController.h / .cpp
        /PawnCharacter.Build.cs
/Content
    /Blueprints
        ABP_NBC_AnimInstance.uasset
        BP_NBC_GameMode.uasset
        BP_NBC_Pawn.uasset
        BP_NBC_PlayerController.uasset
        BP_NBC_UAV.uasset
        BP_NBC_UAVController.uasset
    /Inputs            // Input Action & Input Mapping Context
        IA_Move.uasset
        IA_Look.uasset
        IA_Jump.uasset
        IA_Sprint.uasset
        IA_UpDown.uasset
        IA_Tilt.uasset
        IA_RightClick.uasset
        IMC_Pawn.uasset
        IMC_UAV.uasset

```
