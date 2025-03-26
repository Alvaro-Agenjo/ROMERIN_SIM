


syms q1 q2 q3 q4 q5 q6
syms L1 Lc L4 L5 L6 alpha M_PI

A01 = subs(dh_matrix(q1,0,L1,M_PI/2), M_PI, pi)
A12 = subs(dh_matrix(q2-alpha,0,Lc,M_PI), M_PI, pi)
A23 = subs(dh_matrix(q3-alpha-M_PI/2,0,L5,M_PI/2), M_PI, pi)
A34 = subs(dh_matrix(q4,-L4,0,M_PI/2), M_PI, pi)
A45 = subs(dh_matrix(q5,0,0,M_PI/2), M_PI, pi)
A56 = subs(dh_matrix(q6-M_PI/2,L6,0,0), M_PI, pi)

T=A01*A12*A23*A34*A45*A56;
simplify(T);

syms q3a q2a


A12a = subs(dh_matrix(q2a,0,Lc,M_PI), M_PI, pi);
A23a = subs(dh_matrix(q3a-M_PI/2,0,L5,M_PI/2), M_PI, pi);
A03a=A01*A12a*A23a;
simplify(A03a);
simplify(A03a)
function mat = dh_matrix(theta,d,a,al)
%DH_MATRIX Summary of this function computes 
%   the dh matrix. It si prepared to be symbolic, therefore Q and thetha
%   are decoupled
mat=[cos(theta), -sin(theta) * cos(al), sin(theta)* sin(al),   a* cos(theta);
    sin(theta),  cos(theta)* cos(al),  -cos(theta) * sin(al), a* sin(theta);
		 0,			  sin(al),				cos(al),				 d;
		0, 0, 0, 1];

end


