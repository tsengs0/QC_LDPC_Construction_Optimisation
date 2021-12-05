clc; clear;

filename_0 = 'weight_matrices/weightMatrix.len_4.csv';
filename_1 = 'weight_matrices/weightMatrix.len_6.csv';


Z=96; % the circulant size (or called lifting degree)
base_matrix=[
 0 1 1 0 0 0 0 0 1 1 0 0 1 1 0 0 0 0 0 0 0 0 0 0
 0 1 0 0 0 1 1 1 0 0 0 1 0 1 1 0 0 0 0 0 0 0 0 0
 0 0 0 1 1 1 0 1 0 0 0 1 0 0 1 1 0 0 0 0 0 0 0 0
 1 0 1 0 0 0 0 0 1 1 0 0 0 0 0 1 1 0 0 0 0 0 0 0
 0 0 1 0 0 0 1 0 0 1 1 0 0 0 0 0 1 1 0 0 0 0 0 0
 0 0 0 0 1 1 0 1 0 0 0 1 1 0 0 0 0 1 1 0 0 0 0 0
 0 0 1 1 0 0 0 0 0 1 1 0 0 0 0 0 0 0 1 1 0 0 0 0
 0 1 1 0 0 0 1 0 0 1 0 0 0 0 0 0 0 0 0 1 1 0 0 0
 1 0 0 0 1 1 0 1 0 0 0 1 0 0 0 0 0 0 0 0 1 1 0 0
 0 0 0 0 0 1 0 1 0 0 1 1 0 0 0 0 0 0 0 0 0 1 1 0
 0 0 1 1 0 0 0 0 1 1 0 0 0 0 0 0 0 0 0 0 0 0 1 1
 1 0 0 0 0 1 0 1 0 0 0 1 1 0 0 0 0 0 0 0 0 0 0 1
];

s_num = nnz(base_matrix); % counting the number nonzero elements in base matrix
yU_cycle_4 = 4;
yU_cycle_6 = 6;
U=yU_cycle_4+yU_cycle_6;
f = [zeros(1, s_num) ones(1, U)*0];
%f = transpose(f);

intcon = [1:1:(s_num+U)]; % to specify that all optimised variables are integer 
a_cycle_4 = readtable(filename_0); a_cycle_4=a_cycle_4.Variables;
a_cycle_6 = readtable(filename_1); a_cycle_6=a_cycle_6.Variables;

%% Constraint vector of s_t that 0 <= s_t <= Z-1 for all s_t in {s_1, s_2, ..., s_T}
s_t_constraint = [];
for i=1:1:s_num
    s_t_constraint = [s_t_constraint; zeros(1, U)];
end
s_t_constraint = [eye(s_num) s_t_constraint];

%% Constraint vector of y_u that 0 <= y_u <=1 for all y_u in {y_1, y_2, ..., y_U}
y_u_constraint=[];
for i=1:1:U
    y_u_constraint = [y_u_constraint; zeros(1, s_num)];
end
y_u_constraint = [y_u_constraint eye(U)];

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% To construct the upper bound and lower bound coefficients (to be appeded after each corresponding A(d))
yU_cycle_4_rows=(yU_cycle_4-2)*2+2 + 1;
Z_len_4=Z*4;
Zy_len_4 = zeros(yU_cycle_4_rows, U);
Zy_len_4(1,:)=[-Z_len_4 0 0 0 0 0 0 0 0 0];
Zy_len_4(2,:)=[-Z_len_4 0 0 0 0 0 0 0 0 0];
Zy_len_4(3,:)=[0 -Z_len_4 0 0 0 0 0 0 0 0];
Zy_len_4(4,:)=[0 -Z_len_4 0 0 0 0 0 0 0 0];
Zy_len_4(5,:)=[0 0 -Z_len_4 0 0 0 0 0 0 0];
Zy_len_4(6,:)=[0 0 0 -Z_len_4 0 0 0 0 0 0];
Zy_len_4(7,:)=[1 1 1 1 zeros(1, 6)];

yU_cycle_6_rows=(yU_cycle_6-2)*2+2 + 1;
Z_len_6=Z*5;
Zy_len_6 = zeros(yU_cycle_6_rows, U);
Zy_len_6( 1,:)=[zeros(1, 4) -Z_len_6 0 0 0 0 0];
Zy_len_6( 2,:)=[zeros(1, 4) -Z_len_6 0 0 0 0 0];
Zy_len_6( 3,:)=[zeros(1, 4) 0 -Z_len_6 0 0 0 0];
Zy_len_6( 4,:)=[zeros(1, 4) 0 -Z_len_6 0 0 0 0];
Zy_len_6( 5,:)=[zeros(1, 4) 0 0 -Z_len_6 0 0 0];
Zy_len_6( 6,:)=[zeros(1, 4) 0 0 -Z_len_6 0 0 0];
Zy_len_6( 7,:)=[zeros(1, 4) 0 0 0 -Z_len_6 0 0];
Zy_len_6( 8,:)=[zeros(1, 4) 0 0 0 -Z_len_6 0 0];
Zy_len_6( 9,:)=[zeros(1, 4) 0 0 0 0 -Z_len_6 0];
Zy_len_6(10,:)=[zeros(1, 4) 0 0 0 0 0 -Z_len_6];
Zy_len_6(11,:)=[zeros(1, 4) 1 1 1 1 1 1];
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% To finalise the construction of entire coefficent matrix as constraints
A_cycle_4=[
     a_cycle_4;
    -a_cycle_4;
    -a_cycle_4;
     a_cycle_4;
     a_cycle_4;
    -a_cycle_4;
    zeros(1, s_num)
];
entire_Zy_len_4=[];
for i=1:1:(size(Zy_len_4, 1)-1)
    entire_Zy_len_4=[entire_Zy_len_4; repmat(Zy_len_4(i,:), size(a_cycle_4, 1), 1)];
end
entire_Zy_len_4=[entire_Zy_len_4; Zy_len_4(7,:)];
A_cycle_4=[A_cycle_4 entire_Zy_len_4];
%%
A_cycle_6=[
    -a_cycle_6;
     a_cycle_6;
    -a_cycle_6;
     a_cycle_6;
     a_cycle_6;
    -a_cycle_6;
     a_cycle_6;
    -a_cycle_6;
    -a_cycle_6;
     a_cycle_6;
     zeros(1, s_num)
];
entire_Zy_len_6=[];
for i=1:1:(size(Zy_len_6, 1)-1)
    entire_Zy_len_6=[entire_Zy_len_6; repmat(Zy_len_6(i,:), size(a_cycle_6, 1), 1)];
end
entire_Zy_len_6=[entire_Zy_len_6; Zy_len_6(11,:)];
A_cycle_6=[A_cycle_6 entire_Zy_len_6];
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
A = [
    %% A(0) -> L(d=1) corresponds to cycle path of length-4
    A_cycle_4;

    %% A(1) -> L(d=3) corresponds to cycle path of length-6
    A_cycle_6
];
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
b_path_cycle_4 = [-1 8 -1 8 -10 -10 3];
b_cycle_4=[];
for i=1:1:(size(Zy_len_4, 1)-1)
    b_cycle_4=[b_cycle_4 repmat(b_path_cycle_4(i), 1, size(a_cycle_4, 1))];
end
b_cycle_4=[b_cycle_4 b_path_cycle_4(end)];

b_path_cycle_6 = [-1 8 -10 17 -1 8 -10 17 -19 -19 5];
b_cycle_6=[];
for i=1:1:(size(Zy_len_6, 1)-1)
    b_cycle_6=[b_cycle_6 repmat(b_path_cycle_6(i), 1, size(a_cycle_6, 1))];
end
b_cycle_6=[b_cycle_6 b_path_cycle_6(end)];
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
b_s_t = [ones(1, s_num)*(Z-1)];
b_y_u = [ones(1, U)];
%b = [b_path_1 b_path_2 b_s_t b_y_u];
b = [b_cycle_4 b_cycle_6];
%b = [b_path_1 b_path_1 b_path_2 b_path_2]

% a dummy equality constraint
Aeq = zeros(1, s_num+U);
beq = 0;

lb = zeros((s_num+U),1);
ub = [ones(s_num, 1)*(Z-1); ones(U, 1)*1]; % Enforces x(3) is binary

b = transpose(b);



%x = intlinprog(f, intcon,A, b)
x = intlinprog(f,intcon,A,b,Aeq,beq,lb,ub)

%options = optimoptions(@intlinprog,'OutputFcn',@savemilpsolutions,'PlotFcn',@optimplotmilp);
%x = intlinprog(f,intcon,A,b,Aeq,beq,lb,ub,options);
