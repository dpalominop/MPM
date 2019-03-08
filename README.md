# MPM
Implementation of the Material Point Method using C++ and Cuda.

# Requirements

**OS:** Windows10 <br/>
**IDE:** VisualStudio 2017 <br/>
**Cuda:** 10.0

## How to Build

 *  Open VS 2017 and create a Solution.
 *  Clone the repository inside the directory solution:
 
 ``
 git clone https://github.com/dapalominop/MPM.git
 ``
 
 *  Added the project clonned to solution created before.
 *  Inside the configuration of the project in VisualStudio configure the paths to include and libraries of OpenCV. You could follow this tutorial: [Opencv/VisualStudio2017](http://aprendiendoingenieria.es/instalar-opencv-con-visual-studio-2017/)
 *  Finally, build the project with VisualStudio.
 
 ## Results
 ### Paper Results
 
 <div class="row">
  <div class="column">
    <img src="./static/taichi_total_time.png" width="600">
    
  </div>
  <div class="column">
    <img src="./static/taichi_p2g_porcent.png" width="300">
    <img src="./static/taichi_p2g_number.png" width="300">
  </div>
</div>
 
### Own Implementation Results 

<div class="row">
  <div class="column">
    <img src="./static/cuda_mpm_time.png" width="600">
    
  </div>
</div>
 
 [![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/4m0TCEUNk9Q/0.jpg)](https://youtu.be/4m0TCEUNk9Q)
 
## Contact
If you need any help you can contact me to:
 
**Author:** Daniel Palomino <br/>
**Mail:**   dapalominop@gmail.com
