# Advanced-Lane-Detection with OPENCV
<p align="center"><img src = "https://github.com/junhyukch7/Advanced-Lane-Detection/blob/main/image/result.jpg" width="50%">

<p align="center">Lane Detection Algorithm with Bird Eyes View


## Introduction

Continue after this project(https://github.com/junhyukch7/Lane-Detection)

이전 Lane Detection 프로젝트에서 곡선의 차선의 경우 정확히 인식하지 못하는 문제점이 있었다.

위의 문제점을 보완하기 위해 하늘에서 내려다본 관점에서 차선을 인식하는 알고리즘을 적용하여 해당 문제점을 해결하였다.

---
## Envrionment
Window10(64bit), c++17, OPENCV 4.0.0

---
## Pipline
1. Camera calibration
2. Filtering color(masking yellow and white)
3. GrayScale
4. ROI(Region of Interest)
5. Perspective Transform(Bird's eye view)
6. Sliding Window
7. Linear Interpolation
8. Drawing Line

---

### Step 1 : Camera Calibration
  
실제로 우리가 보는 세상은 3차원이다. 하지만 카메라로 이것을 찍으면 2차원 이미지로 변하게 된다. 이러한 왜곡을 해결하기 위해 보정작업은 필수이다. 체크보드 데이터를 이용하여 왜곡계수를 구하고 해당 계수와 cv::undistort()함수를 이용하여 왜곡을 보정한다.

<p align="center"><img src = "https://github.com/junhyukch7/Advanced-Lane-Detection/blob/main/image/cal_result4.PNG" width="40%">

<p align="center">Finding distortion coefficent task using Check Board
  
|distorted|undistorted|
|------|---|
|<p align="center"><img src = "https://github.com/junhyukch7/Advanced-Lane-Detection/blob/main/image/distorted%20check.PNG" width="70%">|<p align="center"><img src = "https://github.com/junhyukch7/Advanced-Lane-Detection/blob/main/image/undistorted%20check.PNG" width="70%">|
|<p align="center"><img src = "https://github.com/junhyukch7/Advanced-Lane-Detection/blob/main/image/distorted%20image.PNG" width="75%">|<p align="center"><img src = "https://github.com/junhyukch7/Advanced-Lane-Detection/blob/main/image/undistorted%20image.PNG" width="70%">
  
### Step 2 : Filtering Color
  
  차선의 색깔이 희미해지거나 주변색과 유사해질 경우 위와 같이 차선을 인식하지 못하게 된다. 따라서 색을 표현하는 방법을 기존의 RGB에서 HSV로 바꾼다. HSV 이미지에서는 H(Hue)가 일정한 범위를 갖는 순수한 색 정보를 가지고 있기 때문에 RGB 이미지보다 쉽게 색을 분류할 수 있다. 또한 선을 분류할 때 연산량을 줄이기 위해 차선색깔 필터(흰색 노란색)를 적용하였다
  
<p align="center"><img src = "https://github.com/junhyukch7/Advanced-Lane-Detection/blob/main/image/fillter.jpg" width="50%">
  
<p align="center">Masking Yellow and White Color

### Step 3 : GrayScale(bianry)
  
  연산량을 줄이기 위해서 픽셀의 모든 값을 이분법적으로 나눠야 한다. 임계값을 기준으로 기준치 미만인 경우 검은색, 이상인 경우 흰색으로 표현하여 연산량을 줄인다.
  이 과정은 이후 흰색 픽셀만을 찾는 slidingwindow함수의 findNonZero 연산을 할때 유용하다.
  
  <p align="center"><img src = "https://github.com/junhyukch7/Advanced-Lane-Detection/blob/main/image/bin.jpg" width="50%">
    
  <p align="center">binary image
  

### Step 4 : ROI
    
해당 과정은 Bird's eyes view로 transform할 때 기존의 이미지에서 점들을 지정하기 때문에 굳이 거치지 않아도 되는 작업이다.
    
### Step 5 : Perspective Transform
  
<p align="center"><img src = "https://github.com/junhyukch7/Advanced-Lane-Detection/blob/main/image/perspective_transform.PNG" width="15%">
  <p align="center">Bird's Eyes View
  
  ROI과정에서 언급한 것 처럼 기존의 이미지에서 4개의 점을 선택하여 변환을 수행한다. 
  변환하기 전에는 직선 처럼 보이던 선이 변환 후에는 곡선인 것이 확연히 드러나는 것을 알 수 있다.
  
|original|trasfromed|
|------|---|
|<p align="center"><img src = "https://github.com/junhyukch7/Advanced-Lane-Detection/blob/main/image/roi.jpg" width="50%">|<p align="center"><img src = "https://github.com/junhyukch7/Advanced-Lane-Detection/blob/main/image/birdeye.jpg" width="70%">|

### Step 6 : Sliding Window

  가장 많은 시간을 투여한 것 같다... 아이디어는 앞서 전처리 과정에서 binary작업을 수행했기 때문에 차선이 있는 곳은 흰색이고 없는 곳은 검정색으로 표현될 것이다.
  cv::findNonZero 함수를 이용하여 0이 아닌 픽셀의 벡터를 반환한다. 이후 이 벡터의 x좌표를 이용하여 평균x좌표를 구하고 이 좌표에서 y값을 증가시키며 slinding window작업을 수행한다. 
    
|Left|Right|
|------|---|
|<p align="center"><img src = "https://github.com/junhyukch7/Advanced-Lane-Detection/blob/main/image/windows1.jpg" width="70%">|<p align="center"><img src = "https://github.com/junhyukch7/Advanced-Lane-Detection/blob/main/image/windows.jpg" width="70%">|
|<p align="center"><img src = "https://github.com/junhyukch7/Advanced-Lane-Detection/blob/main/image/windowsR2.jpg" width="70%">|<p align="center"><img src = "https://github.com/junhyukch7/Advanced-Lane-Detection/blob/main/image/windowsR1.jpg" width="70%">
  
### Step 7 : Linear interpolation
  
  slding window 결과 흰색 픽셀이 위치한 좌표를 얻게 된다. 이 좌표들을 기반으로 선형 보간법을 수행한다. 다항식 보간법은 고차항으로 갈수록 오차가 커질 가능성이 있어 선형 보간법을 선택했다. 
  
|Curve|Straight|
|------|---|
|<p align="center"><img src = "https://github.com/junhyukch7/Advanced-Lane-Detection/blob/main/image/birdeyeline.jpg" width="80%">|<p align="center"><img src = "https://github.com/junhyukch7/Advanced-Lane-Detection/blob/main/image/pro.jpg" width="80%">|
  
### Step 8 : Drawing Line
  
  Bird's eye view에서 만든 점들을 original 공간의 좌표형태로 바꿔준 후 선을 그린다. 
  
  <pre><code>perspectiveTransform(pts, outPts, invertedPerspectiveMatrix); //Transform points back into original image space</code></pre>
 
|Curve|Straight|
|------|---|
|<p align="center"><img src = "https://github.com/junhyukch7/Advanced-Lane-Detection/blob/main/image/result.jpg" width="77%">|<p align="center"><img src = "https://github.com/junhyukch7/Advanced-Lane-Detection/blob/main/image/src.jpg" width="100%">|
  
---
## Result video
  
|Non-Bird-eye(before)|Bird-eye(after)|
|------|---|
|<p align="center"><img src = "https://github.com/junhyukch7/Advanced-Lane-Detection/blob/main/image/challenge.gif" width="80%">|<p align="center"><img src = "https://github.com/junhyukch7/Advanced-Lane-Detection/blob/main/image/birdeye2.gif" width="100%">|
 
이전 프로젝트에서 설계한 알고리즘은 곡선영역에서 차선을 인식할때 매우 심한 진동을 보이며 차선을 인식한다. 이는 매우 불안정한 모습이며 제어 단계에서도 악영향을 미칠 가능성이 있었다.
반면에 이번 프로젝트에서 설계한 알고리즘은 곡선 영역에서도 큰 진동없이 차선을 잘 인식한 것을 볼 수 있다. 신뢰도가 높은 좌표 데이터를 기반으로 한 선형보간법을 이용하여 기존에 있던
진동 문제를 해결할 수 있었다.


