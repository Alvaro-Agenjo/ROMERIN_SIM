function [q] = m2q(m)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
factor4 = 24.0 / 32.0 ;
factor6 = 34.0 / 51.0 ;
Pdeg2rad = pi/180;

q1 = (m(1) - 180) * Pdeg2rad;
q2 = (m(2) - 180) * Pdeg2rad;
q3 = (m(3) - 90) * Pdeg2rad;
q4 = (m(4) - 180) * factor4 * Pdeg2rad;
q5 = (m(5)- m(6)) * Pdeg2rad / 2 ;
q6 = (180 - (m(5)+ m(6))/2) * factor6 * Pdeg2rad;

q=[q1, q2, q3, q4, q5, q6];
end