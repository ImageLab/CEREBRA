
% Doc example  Chapter 5.

% Copyright 1999-2014 The MathWorks, Inc.

% This file uses the %#external pragma, which is no longer supported
% as of R2015a. However, we retain the file for test purposes.

function collect

    y = zeros(1,100); % pre-allocate the matrix 
    for i = 1:100 
        y(i) = collect_one; 
    end

    
    
function y = collect_one
    %#EXTERNAL
    persistent t;
 
    if (isempty(t)) 
        t = 0; 
    else 
        t = t+0.05; 
    end 
    y = sin(t);
