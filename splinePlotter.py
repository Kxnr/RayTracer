import matplotlib.pyplot as mp	
import numpy as np

class Spline:
    def Spline(self, pts, times):
        self.n = len(pts)
        if n < 4:
            raise Exception("Not enough points")

        self.pts = pts
        self.times = times

    def _get_params():
        self.params = np.array((4*self.n-4, 3))
        # TODO:


        pass
        

    def position(t):
        for i in range(len(times)):
            if t < times[i]:
                break

        part = i - 1
        part = 0 if (part < 0) else n-2 if (part > n-2)  else part

        # turn part into index
        part *= 4

        return self.params[part,:] + self.params[part+1,:] * t + self.params[part+2]*t*t + self.params[part+3] * t * t * t
    

    

if __name__ == "__main__":
    # TODO: define points in txt file
    pass    