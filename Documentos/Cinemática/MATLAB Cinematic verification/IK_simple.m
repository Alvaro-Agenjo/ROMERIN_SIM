function [q] = IK_simple (x, y, z, elbow)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
L1 = 0.068;
L2 = 0.236;
L3 = 0.0149;
L4 = 0.28;
L5 = 0.022;
L6 = 0.087;

Lc = sqrt(L2 * L2 + L3 * L3);
Ld = sqrt(L4 * L4 + L5 * L5);
alpha = atan2(L3,L2);
beta = atan2(L5,L4);
if (elbow == 0) 
    elbow = -1;
end
%% 

q1 = atan2(y, x);
x = x - L1 * cos(q1);
y = y - L1 * sin(q1);

num = (x * x + y * y + z * z) - (Lc * Lc) - (Ld * Ld);
den = 2 * Lc * Ld;
absNum = abs(num);
if(absNum > den) 
    q = [nan,nan, nan];
else
    q3_ = elbow * acos(num / den);
    q2_ = atan2(z, sqrt(x * x + y * y)) + elbow * atan2(Ld * sin(q3_), Lc + Ld * cos(q3_));

    q2 = q2_ + alpha;
    q3 = q3_ + (alpha + beta);
    q = [q1, q2, q3];
end

end