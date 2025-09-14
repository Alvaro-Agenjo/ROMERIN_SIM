function [m] = CalcROT(a, b, c)
%UNTITLED4 Summary of this function goes here
%   Detailed explanation goes here

a =deg2rad(a);
b =deg2rad(b);
c =deg2rad(c);


cx = cos(a); sx = sin(a);
cy = cos(b); sy = sin(b);
cz = cos(c); sz = sin(c);

orientacion = zeros(3,3);
%// Matriz de rotación sobre ejes globales (Rx * Ry * Rz)
orientacion(1,1) = cy * cz;
orientacion(1,2) = -cy * sz;
orientacion(1,3) = sy;

orientacion(2,1) = sx * sy * cz + cx * sz;
orientacion(2,2) = -sx * sy * sz + cx * cz;
orientacion(2,3) = -sx * cy;

orientacion(3,1) = -cx * sy * cz + sx * sz;
orientacion(3,2) = cx * sy * sz + sx * cz;
orientacion(3,3) = cx * cy;

m = orientacion;
end