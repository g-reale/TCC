function export(path, fir, iir = [])
    fid = fopen(path,'w');
    is_complex = any(imag(fir) != 0) || any(imag(iir) != 0);
    
    for k = 1:length(fir)
        if is_complex
            fprintf(fid, "(%.9f,%.9f) ", real(fir(k)), imag(fir(k)));
        else
            fprintf(fid, "%.9f ", real(fir(k)));
        end
    endfor
    fprintf(fid, "\n");

    for k = 1:length(iir)
        if is_complex
            fprintf(fid, "(%.9f,%.9f) ", real(iir(k)), imag(iir(k)));
        else
            fprintf(fid, "%.9f ", real(iir(k)));
        end
    endfor
    fprintf(fid, "\n");
    fclose(fid);
endfunction