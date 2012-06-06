% generate data for fft

clear;

for i = 10:20;
    val = zeros(2^i, 1);
    for j=1:(2^i);
        val(j) = sin(1 + j * pi / (2^(i - 8))) + 4 * sin(2.1 + j * pi / (2^(i - 6)));
    end;
    file_name = sprintf('input-%02d.txt', i);
    save(['~/Desktop/PA/fft/' file_name], 'val', '-ascii');
end;

system('ls -lah ~/Desktop/PA/fft/*.txt', '-echo');