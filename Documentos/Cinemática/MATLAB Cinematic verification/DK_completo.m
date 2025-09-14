function [p, m] = DK_completo(q)
%UNTITLED3 Summary of this function goes here
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


q1 = q(1); q2 = q(2) - alpha; q3 = q(3) - alpha; q4 = q(4); q5 = q(5); q6 = q(6);
c1 = cos(q1); s1 = sin(q1); c2 = cos(q2); s2 = sin(q2);
s3_2 = sin(q3-q2); c2_3 = cos(q2 -q3);
c4 = cos(q4); s4 = sin(q4); c5 = cos(q5); s5 = sin(q5); c6 = cos(q6); s6 = sin(q6);

A03 = [[c1 * s3_2              , -s1                     , -c1 * c2_3,  c1 * (L5 * s3_2 + Lc * c2 + L1)];
       [s1 * s3_2              ,  c1                     , -s1 * c2_3,  s1 * (L5 * s3_2 + Lc * c2 + L1)];
       [c2_3                   ,  0                      , s3_2      ,  L5 * c2_3 + Lc * s2            ];
       [0                      ,  0                      , 0         ,  1                             ]];
A36 = [[c4 * c5 * s6 - s4 * c6 , c4 * c5 * c6 + s4 * s6  , c4 * s5   ,  L6 * c4 * s5];
       [s4 * c5 * s6 + c4 * c6 , s4 * c5 * c6 - c4 * s6  , s4 * s5   ,  L6 * s4 * s5];
       [s5 * s6                , s5 * c6                 , -c5       , -L6 * c5 - L4];
       [0                      ,  0                      ,  0        ,  1          ]];

A06 = A03 * A36;
p = A06(1:3, 4)';
m = A06(1:3, 1:3);
end