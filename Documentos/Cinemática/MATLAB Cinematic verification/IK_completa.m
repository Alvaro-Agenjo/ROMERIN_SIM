function [q] = IK_completa(m, p, elbow)
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


pm = [p(1)- L6 * m(1,3), p(2) - L6 * m(2,3), p(3) - L6 * m(3,3)];

q = IK_simple(pm(1), pm(2), pm(3), elbow);
if isnan(q)
    q = [nan, nan, nan, nan, nan, nan];
end

q1 = q(1); q2 = q(2) - alpha; q3 = q(3) - alpha;
s1 = sin(q1); c1 = cos(q1);
c2_3 = cos(q2 - q3); s3_2 = sin(q3 - q2);
A=[[c1 * s3_2, -s1 , -c1* c2_3]; %A03
   [s1 * s3_2,  c1 , -s1* c2_3];
   [c2_3     ,   0 ,      s3_2]];

R = zeros(3,3); %R = A.trasp * m
for i = 1:3
    R(i,1) = A(1,i) * m(1,1) + A(2,i) * m(2,1) + A(3,i) * m(3,1);
    R(i,2) = A(1,i) * m(1,2) + A(2,i) * m(2,2) + A(3,i) * m(3,2);
    R(i,3) = A(1,i) * m(1,3) + A(2,i) * m(2,3) + A(3,i) * m(3,3);
end
%    //si q5!=0 ojo.
if (abs(R(3,3)) < 0.99) 
    %//dando prioridad a q4 (-pi/2, pi/2), por lo que c4>0
    if (R(1,3) > 0) %//q5 >0
        q(4) = atan2(R(2,3), R(1,3));
        q(5) = acos(-R(3,3));
        q(6) = atan2(R(3,1), R(3,2));
    else 
        q(4) = atan2(-R(2,3), -R(1,3));
        q(5) = -acos(-R(3,3)); %//esto impide abs(q5) > pi/2
        q(6) = atan2(-R(3,1), -R(3,2));
    end
else
    q = [nan, nan, nan, nan, nan, nan];
end