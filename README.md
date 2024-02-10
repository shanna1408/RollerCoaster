# CPSC 687 Assignment 1
# Shanna Hollingworth Winter 2024

## Instructions:

* To run this program, click the a1_base.exe inside of the build/Debug folder (Compiled for Windows 11 using Visual Studio C++ compiler).

* To start the animation, click `Play`. Once the animation has started, a `Pause` option will appear.

* Use the `Background Colour` selector to change the background colour of the animation.

* The `Playback Speed` option allows you to control the speed at which the animation plays. The default setting is 1 which is also the lowest setting, and the highest option is 10.

* The `Look Ahead` slider allows you to configure the wideness of the curve samples at any given point. The smallest option 1 is, and the largest is 10. The default setting is at 5.

* Reset view will return the camera to it's original position.

* Reset simulation will move the cart back to it's starting position.

* The only model available is `"roller_coaster_1.obj"`, which is loaded by default.

## Hermite Curve

The first part of this solution was evaluating a cubic hermite curve. In order to achieve this, we can derive and simplify our curve equations into a singular expression which allows us to find an exact point in between two control points, using those points and their tangents. The expression is as follows:

$$
C(u_i) = p_i(2u_i^3 - 3u_i^2 + 1) + t_i(u_i^3 - 2u_i^2 + u_i) + p_{i+1}(-2u_i^3 + 3u_i^2) + t_{i+1}(u_i^3-u_i^2)
$$

Now given some u between 0 and 1, we can interpolate between the control points using the expression and find an exact position on the curve.

## Arc Length Paramaterization

In order to ensure that we are stepping evenly along the curve for accurate movement at a constant speed, we must now arc length parameterize the curve. For this, we pick two very small values delta s and delta u, and use the following algorithm to create a table full of arc length paramterized values for easy access:

```c++
float L_C = length of the curve;
int N = floor(L_C/delta_s);	
ArcLengthTable table(delta_s);
table.reserve_memory(N);
table.addNext(0);
float dS_accum = 0;
float u = 0;

while (u <= 1) {
	dS_accum = dS_accum + (distance between position on curve at u+delta_u and at u)
	if (dS_accum > delta_s) {
		table.addNext(u + delta_u);
		dS_accum = dS_accum - delta_s;
	}
	u = u + delta_u;
	}
	return table;
```

Now that we have an arc length paramaterization table, with a variable 's' corresponsing to each index, given any value s which represents our distance along the curve we can find the closest ALP values above and below s, and then interpolate to find the exact position.

## Getting the speed of the cart

In order to calculate the speed of the cart, I first needed to find the highest point on the curve 'H' as well as the position on the curve where it occured, 'topS'. I calculated and stored this while calculating the ALP table. 

Now that we have this value, there were one of three cases:

1. The lifting phase, which I defined as any point between the starting position on the track (s=0) to the highest point of the track (s=topS). For this portion, the cart maintains a slow constant speed called the chain speed.

2. The freefall phase, which is any point after the highest point and before the decelaration phase(which encompasses the last 5% of the track.) In the freefall phase, the speed is calculated using the formula $\sqrt{2g(H-h)}$ where 'h' is the current height. Because this calculation can sometimes yield very small values, in particular when the difference between H and h is small, I set my algorithm to select the maximum value between this speed and the chain speed.

3. Finally, we have the deceleration phase. This is where our cart smoothly slows down and stops, before taking off again. In order to calculate the speed at this stage, I used the formula (vdec - 0) * (ddec / Ldec), where vdec is the speed upon entering the deceleration phase, Ldec is the full length of the deceleration phase, and ddec is the remaining length of the deceleration phase. When the cart reaches a stop, a timer counts to three seconds (Which may not be enough for people to actually leave and eneter the cart, but it's enough to get the point!), and then the speed is reset to the chain speed and we loop back to the lifting phase.

# Framing the curve
In order to get the direction of the curve to properly place the track pieces and the cart, I used a frenet frame. Essentially, we want to calculate the centripetal acceleration at each point along the curve, which we can later use to calculate the Normal vector. We can use the equation a=kn, where k is the curvature of the oscillating circle at the point and n is the direction of the acceleration.

Using some lookahead value 'h' which is defaulted to '5' but may be altered by the user to any value between 1 and 10, we first calculated the vectors from our position 'x' taken from the ALP table at a distance s, and the position h distance ahead of and behind x, dubbed 'a' and 'b'. Now, we are able to calculate a third vector 'c' by taking the difference of those two vectors and normalizing the result, which gives us tangent line 'T' at the cart's current position 

Now we can find the direction n, by normalizing a to get 't0' and b to get 't1' and then evaluating and normalizing the vector t1-t0. Due to the approximations in calculations we still must orthogonalize n by removing its parallel component.

Now, given that curvature k is an inverse of the radius 'r' such that k=1/r, and that r can be found using the inscibed angle theorem to be c/(2\*length(t1 cross t0)). Therefore, k=(2\*length(t1 cross t0))/c.

As all the necessary components to calculate the centripetal acceleration have now been obtained, we are able to evaluate the centripetal acceleration. Finally, we can calculate the normal vector 'N' by removing the orthogonalized gravity vector from the acceleration, and normalizing the result.

Now, we have the normal vector 'N' and the tangent vector 'T' at the cart's position; all that's left is to calculate the binormal vector, which evaluates to N cross T. Using these three vectors and the position x on the curve, we can produce a four dimensial matrix M, which is used to define the position and orientation of the cart.

To place the track pieces, we instead divide the curve into 1000 segments and step through the segments, doing the above calculations at each point to determine the position and orientation of each track piece.

# Bonus

The multiple cart bonus was done by calculating a different s for each cart decremented by 1.5 from the s value of the front cart. There is a bug at the end of the curve where the carts appear to blur into each other momentarily; this is not due to the implementation of the multiple carts, but rather a result of an even weirder bug where as each cart passes from the end of the curve to the start, the cart is teleported backwards ever so slightly.
