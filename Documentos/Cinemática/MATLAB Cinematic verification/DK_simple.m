function [p] = DK_simple(q)
%UNTITLED4 Summary of this function goes here
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


c4 = cos(0); s4 = sin(0);
q1 = q(1); q2 = q(2) - alpha; q3 = q(3) - alpha;
c1 = cos(q1); s1 = sin(q1); c2 = cos(q2); s2 = sin(q2);
s3_2 = sin(q3-q2); c2_3 = cos(q2 -q3);

A03= [[c1 * s3_2, -s1, -c1 * c2_3, c1 * (L5 * s3_2 + Lc * c2 + L1)];
      [s1 * s3_2,  c1, -s1 * c2_3, s1 * (L5 * s3_2 + Lc * c2 + L1)];
      [c2_3, 0, s3_2, L5 * c2_3 + Lc * s2];
      [0,0,0,1]];
A34 = [[c4 ,0, s4 ,0];
       [s4, 0, -c4, 0];
       [0, 1, 0, -L4];
       [0,0,0,1]];
A04 = A03 * A34;

p = A04(1:3, 4);
end