M_PI = pi;
L1=0.068
L2=0.236
L3=0.0149
L4=0.280
L5=0.022
L6=0.087
Lc=sqrt(L2^2+L3^2)
alpha = atan2(L3,L2)
Ld=sqrt(L4^2+L5^2)
beta= atan2(L5,L4)

L(1)=Link([0,			0,		L1,		   M_PI/2]);
L(2)=Link([0,			0,		Lc,	       M_PI , 0, -alpha ]);
L(3)=Link([0,           0,		L5,	       M_PI/2, 0,  -M_PI/2-alpha]);
L(4)=Link([0,			-L4,	0,		   M_PI/2]);

Lb(1)=Link([0,			0,		L1,		M_PI/2]);
Lb(2)=Link([0,			0,		Lc,     M_PI ]);
Lb(3)=Link([0,           0,		0,     M_PI/2, 0,  -M_PI/2]);
Lb(4)=Link([0,			-Ld,	0,		M_PI/2]);

Lf(1)=Link([0,			0,		L1,		   M_PI/2]);
Lf(2)=Link([0,			0,		Lc,	       M_PI , 0, -alpha ]);
Lf(3)=Link([0,           0,		L5,	       M_PI/2, 0,  -M_PI/2-alpha]);
Lf(4)=Link([0,			-L4,	0,		   M_PI/2]);
Lf(5)=Link([0,           0,		0,	       M_PI/2]);
Lf(6)=Link([0,           L6,		0,	       0, 0,  -M_PI/2]);
pata = SerialLink(L);
pata2 = SerialLink(Lb);
pataf = SerialLink(Lf);
q = [0,0, 0,0;
    M_PI / 2,0, 0,0;
    M_PI / 2,M_PI / 2, 0,0;
    M_PI / 2,M_PI / 2, M_PI / 2,0
    ] ;
for i = 1:4
 T=pata.fkine(q(i,:));
 T.t'
 T2=pata2.fkine(q(i,:)+[0 -alpha -alpha-beta 0]);
 T2.t'
end
"pata.teach()"
pataf.teach()