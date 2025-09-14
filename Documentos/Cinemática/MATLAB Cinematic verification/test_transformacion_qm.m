
clear
clc

i=0;
%%
r180 = -180:10:180; r180 = r180*pi/180;
r360 = 0:10:360; r360 = r360*pi/180;
n= 0; bool = 1;
for n = r360
    for p = r180
        i= i+1;
        m(i,:) = q2m([0,0,0,0,p,n]);
        q(i,:) = m2q(m(i,:));

        q2(i,:) = [p,n];
        if any(abs(q2(i,:)- q(i, 5:6)) > 0.0001)
            fprintf('fail \n');
            bool = 0;
        end
    end
end

if bool == 1
    fprintf('Todo ok')
end