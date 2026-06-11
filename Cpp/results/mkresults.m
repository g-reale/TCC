function [a,b] = mkresults(path,color,linewidth)
    
    hold on;
    data = dlmread(path,';');
    
    %original data
    n = data(:,1);
    times = data(:,2);
    loglog(n,times,'Color',color,'LineWidth', linewidth);

    %linear regression
    n = log2(n);
    times = log2(times);
    mask = isfinite(n) & isfinite(times);
    n = n(mask);
    times = times(mask);
    poly = polyfit(n, times, 1);
    fit = polyval(poly,n);
    loglog(2.^n, 2.^fit, '--','Color',color,'LineWidth', linewidth);

    %return coefficients
    a = poly(1);
    b = 2^poly(2);
endfunction