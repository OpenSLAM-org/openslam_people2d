function h = foo(fname)
% /*****************************************************************
%  *
%  * This file is part of the People2D project
%  *
%  * People2D Copyright (c) 2011 Luciano Spinello
%  *
%  * This software is licensed under the "Creative Commons 
%  * License (Attribution-NonCommercial-ShareAlike 3.0)" 
%  * and is copyrighted by Luciano Spinello
%  * 
%  * Further information on this license can be found at:
%  * http://creativecommons.org/licenses/by-nc-sa/3.0/
%  * 
%  * People2D is distributed in the hope that it will be useful,
%  * but WITHOUT ANY WARRANTY; without even the implied 
%  * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
%  * PURPOSE.  
%  *
%  *****************************************************************/

data = load(fname);
figure(1);
hold on, grid on;
h = plot(data(:,2), data(:,1), 'r-', 'LineWidth', 2);
ylabel('Recall')
xlabel('Precision')
axis([0 1 0 1]);

