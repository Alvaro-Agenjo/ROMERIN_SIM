clear;
clc;
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
% %%
% 
% [p, m] = DK_completo([0,1.2,1.5,-0.5,2,-1.3]);
% q = IK_completa(m, p, 1); 
%%
bool = 1;
p = 0;
x = 0.4; y = 0.1; z = 0.1;
for a = 0:5:360
    for b = 0:5:360
        for c = 0:5:360
            p = p +1;
            m = CalcROT(a,b,c);
            q = IK_completa(m, [x,y,z], 1);
            almacen(p,:)= q;
            [pos, rot] = DK_completo(q);
            
            if any(abs(pos -[x,y,z]) > 0.00001)
                fprintf('Fallo en posicion \n');
                fprintf('pos origin %f, %f, %f, pos obtenida %f, %f, %f', x, y, z, pos);
                bool = 0;
            elseif any(abs(m-rot) > 0.0001)
                fprintf('Fallo en orientacion\n');
                fprintf('pos error %f, %f, %f, y orientacion %f %f %f', x, y, z, a, b, c);
                bool = 0;
            end
        end
    end
end

if bool == 1
    fprintf('Todo ok');
end

%%
for n = 1:size(almacen,1)
    motor(n,:) = q2m(almacen(n,:));
    nuevaq(n,:) = m2q(motor(n,:));

    if any(abs(almacen(n,:)-nuevaq(n,:)) > 0.0001)
        fprintf(' error en iteracion %d', n);
    end
end
