# Advanced-Lane-Detection with OPENCV
<p align="center"><img src = "https://github.com/junhyukch7/Advanced-Lane-Detection/blob/main/image/result.jpg" width="50%">

<p align="center">Lane Detection Algorithm with Bird Eyes View


## Introduction

Continue after this project(https://github.com/junhyukch7/Lane-Detection)

이전 Lane Detection 프로젝트에서 곡선의 차선의 경우 정확히 인식하지 못하는 문제점이 있었다.

위의 문제점을 보완하기 위해 하늘에서 내려다본 관점에서 차선을 인식하는 알고리즘을 적용하여 해당 문제점을 해결하였다.

---
## Envrionment
Window10(64bit), c++17, OPENCV4.0.0

---
## Pipline
1. Camera calibration
2. Filtering color(masking yellow and white)
3. GrayScale
4. ROI(Region of Interest)
5. Perspective Transform(Bird's eye view)
6. Sliding Window
7. linear interpolation(고차항 갈수록 오차가 커져 선형보간법 선택)
8. drawing Line

---

### Step 1
* Camera Calibration
실제로 우리가 보는 세상은 3차원이다. 하지만 카메라로 이것을 찍으면 2차원 이미지로 변하게 된다. 이러한 왜곡을 해결하기 위해 보정작업은 필수이다. 체크보드 데이터를 이용하여 왜곡계수를 구하고 해당 계수와 cv::undistort()함수를 이용하여 왜곡을 보정한다.

<p align="center"><img src = "https://github.com/junhyukch7/Advanced-Lane-Detection/blob/main/image/cal_result4.PNG" width="40%">

<p align="center">Check board undistorted task
  
|distorted|undistorted|
|------|---|
|<p align="center"><img src = "https://github.com/junhyukch7/Advanced-Lane-Detection/blob/main/image/distorted%20check.PNG" width="70%">|<p align="center"><img src = "https://github.com/junhyukch7/Advanced-Lane-Detection/blob/main/image/undistorted%20check.PNG" width="70%">|
|<p align="center"><img src = "https://github.com/junhyukch7/Advanced-Lane-Detection/blob/main/image/distorted%20image.PNG" width="75%">|<p align="center"><img src = "https://github.com/junhyukch7/Advanced-Lane-Detection/blob/main/image/undistorted%20image.PNG" width="70%">
  
### Step 2
* Filtering Color(preprocessed)
  차선의 색깔이 희미해지거나 주변색과 유사해질 경우 위와 같이 차선을 인식하지 못하게 된다. 따라서 색을 표현하는 방법을 기존의 RGB에서 HSV로 바꾼다. HSV 이미지에서는 H(Hue)가 일정한 범위를 갖는 순수한 색 정보를 가지고 있기 때문에 RGB 이미지보다 쉽게 색을 분류할 수 있다. 또한 선을 분류할 때 연산량을 줄이기 위해 차선색깔 필터(흰색 노란색)를 적용하였다
  
<p align="center"><img src = "https://github.com/junhyukch7/Advanced-Lane-Detection/blob/main/image/fillter.jpg" width="50%">
  
<p align="center">Masking Yellow and White Color

### Step 3
* GrayScale(bianry)
  연산량을 줄이기 위해서 픽셀의 모든 값을 이분법적으로 나눠야 한다. 임계값을 기준으로 기준치 미만인 경우 검은색, 이상인 경우 흰색으로 표현하여 연산량을 줄인다.
  이 과정은 이후 흰색 픽셀만을 찾는 slidingwindow함수의 findNonZero 연산을 할때 유용하다.
  
  <p align="center"><img src = "https://github.com/junhyukch7/Advanced-Lane-Detection/blob/main/image/bin.jpg" width="50%">
    
  <p align="center">binary image
  


---



