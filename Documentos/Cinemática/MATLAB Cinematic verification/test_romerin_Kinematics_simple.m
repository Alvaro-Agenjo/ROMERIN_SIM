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
%%
p = 0;
for l = 0.2:0.01:0.7
    for m = 0.2:0.01:0.7
        for n = -0.4:0.01:0.4
            p = p +1;
            q(p, :) = IK_simple(l,m,n,1);
            if isnan(q(p,:))
                pointsIK(p,:) = [nan, nan, nan];
            else
                pointsIK(p,:) = [l, m, n];
            end
        end
    end
end
% %%
% figure;
% hold on;
% plot(0:0.005:0.7, q(:, 2));
% plot(0:0.005:0.7, q(:, 3));
% legend("q2", "q3");
% title("Q2 y Q3");
% hold off;

%%
for n = 1: size(q, 1)
    pointsDK(n, :) = DK_simple(q(n, : ));
end

%%
bool = 1;
for i = 1:size(pointsDK,1)
    if isnan(pointsDK(i,:)) 
    elseif any(abs(pointsDK(i,:) - pointsIK(i,:)) > 0.000001)
        fprintf('Diferencia en punto: %f\n', pointsIK(i,1));
        fprintf('IK = %f, %f, %f;  \nDK = %f, %f, %f; \n', pointsIK(i,:), pointsDK(i,:));
        bool = 0;
    end
end

if bool == 1
    fprintf('Todo correcto');
end