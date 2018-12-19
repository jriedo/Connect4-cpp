close all;
depth = [4,6,8,10,12];
% without multithreading, pure processing time
times_ms = [15,200,3500,34500,267750]/1000;
times_threading = [18,100,2000,20500, 195500]/1000;
times_python = [4100, 40500, 796000]/1000;

semilogy (depth(1:3), times_python, 'linewidth', 1.5)
hold on
semilogy (depth, times_ms, 'linewidth', 1.5)
semilogy (depth, times_threading, 'linewidth', 1.5)
xlabel('depth')
ylabel('processing time [s]')
grid on





legend('python', 'c++ baseline', 'c++ thread', 'location', 'southeast')
