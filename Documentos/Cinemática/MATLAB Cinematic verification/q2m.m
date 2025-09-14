function [m] = q2m(q)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
factor4 = 24.0 / 32.0 ;
factor6 = 34.0 / 51.0 ;
Prad2deg = 180/pi;

m1 = 180 + Prad2deg * q(1);
m2 = 180 + Prad2deg * q(2);
m3 = 90  + Prad2deg * q(3);
m4 = 180 + Prad2deg * q(4) / factor4;
m5 = 180 + Prad2deg * (q(5) - q(6)/factor6);
m6 = 180 - Prad2deg * (q(5) + q(6)/factor6);
m = [m1, m2, m3, m4, m5, m6];
end