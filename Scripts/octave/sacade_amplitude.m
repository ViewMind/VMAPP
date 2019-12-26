x0 = 683.842;
y0 = 1017.84; 
x = 962.853; 
y = 1014.65;
xMM2Px = 2/1352;
yMM2Px = 2/1704;

dxInMM = (x - x0)*xMM2Px;
dyInMM = (y - y0)*xMM2Px;
delta = sqrt(dxInMM^2 + dyInMM^2);

sacade = atan(delta/2(wh)*180/pi