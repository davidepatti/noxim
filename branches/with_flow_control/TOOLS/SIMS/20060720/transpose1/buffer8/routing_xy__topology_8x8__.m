% fname: routing_xy__topology_8x8__.m
% ./noxim -routing xy -dimx 8 -dimy 8  -sim 10000 -warmup 2000 -size 8 8 -buffer 8 -traffic transpose1 

function [max_pir, max_throughput, min_delay] = routing_xy__topology_8x8__(symbol)

data = [
%             pir      avg_delay     throughput      max_delay       rpackets         rflits
            0.007        25.2238      0.0561543            248           3592          28751
            0.007        23.2993      0.0541113            291           3461          27705
            0.007        21.9363      0.0547227            236           3501          28018
            0.007        26.7828      0.0553848            535           3545          28357
            0.007        21.7244      0.0540039            156           3454          27650
            0.007        24.6274      0.0560508            447           3586          28698
            0.007        24.2444      0.0547559            338           3503          28035
            0.007        23.0121      0.0556914            232           3562          28514
            0.007        25.9559      0.0559844            385           3581          28664
            0.007        25.6116      0.0549023            460           3514          28110
            0.007        24.3263      0.0546074            245           3494          27959
            0.007        28.5365      0.0564883            386           3616          28922
            0.007          26.01      0.0560664            317           3592          28706
            0.007        31.2215      0.0583672            685           3733          29884
            0.007        22.2647      0.0548828            253           3514          28100
            0.007        26.4447      0.0557109            424           3564          28524
            0.007         24.721      0.0552246            319           3530          28275
            0.007        23.7251       0.054707            282           3503          28010
            0.007        25.8891      0.0553457            550           3544          28337
            0.007        23.9345      0.0563008            329           3602          28826
            0.008        29.7561      0.0637324            263           4079          32631
            0.008        37.1441      0.0628691           1372           4025          32189
            0.008        31.0398       0.063957            804           4095          32746
            0.008        34.3093      0.0642598            726           4112          32901
            0.008        35.7741      0.0628008            602           4020          32154
            0.008        28.4537      0.0635332            418           4069          32529
            0.008        39.4741      0.0633691           1362           4054          32445
            0.008        33.5907      0.0640449            820           4100          32791
            0.008        30.2767      0.0635664            428           4066          32546
            0.008        36.8859      0.0657266            617           4205          33652
            0.008        41.4636      0.0644902           1184           4126          33019
            0.008          37.87      0.0641836            876           4107          32862
            0.008        31.3065      0.0644297            590           4121          32988
            0.008        34.6784      0.0629082           1162           4024          32209
            0.008         33.809      0.0639023            870           4088          32718
            0.008        32.2556      0.0636836            801           4076          32606
            0.008        43.1011      0.0641562            852           4105          32848
            0.008        34.8015      0.0645645            472           4126          33057
            0.008        32.6632      0.0636484            456           4074          32588
            0.008        28.2957      0.0627148            554           4011          32110
            0.009        80.5591      0.0702637           3261           4498          35975
            0.009        63.0502      0.0725645           1644           4643          37153
            0.009         58.351       0.071543           2359           4576          36630
            0.009        81.4321      0.0703711           2536           4506          36030
            0.009        97.7063       0.071416           2508           4573          36565
            0.009         129.01      0.0722031           3600           4622          36968
            0.009        52.4893      0.0716953           1806           4592          36708
            0.009        68.1689      0.0725527           1563           4643          37147
            0.009        78.7786      0.0723105           2671           4626          37023
            0.009        53.9784      0.0722187           1747           4622          36976
            0.009        67.8621       0.072334           3481           4628          37035
            0.009        79.8701      0.0724512           2245           4635          37095
            0.009        71.8365      0.0700547           2687           4482          35868
            0.009        84.9238      0.0713379           1942           4565          36525
            0.009        67.3613      0.0712656           1692           4564          36488
            0.009        60.5529      0.0723672           1449           4628          37052
            0.009        100.893      0.0711074           2774           4547          36407
            0.009        103.567      0.0723965           4293           4633          37067
            0.009        110.684      0.0715937           4455           4583          36656
            0.009         63.758      0.0708379           2057           4533          36269
             0.01        137.817      0.0771797           6792           4941          39516
             0.01        157.909      0.0764473           5417           4893          39141
             0.01        110.834      0.0773301           4061           4948          39593
             0.01         153.77      0.0790332           6239           5058          40465
             0.01        186.697      0.0784336           5180           5020          40158
             0.01        184.126      0.0795781           6330           5091          40744
             0.01        140.341       0.078125           9519           4999          40000
             0.01        176.503      0.0764785           6814           4898          39157
             0.01        97.2062      0.0777988           6595           4980          39833
             0.01        129.002      0.0779746           7138           4990          39923
             0.01        177.523      0.0778105           7127           4980          39839
             0.01        143.166      0.0795059           5193           5090          40707
             0.01        135.252      0.0785234           4782           5024          40204
             0.01        152.842      0.0775957           6139           4965          39729
             0.01        110.433       0.078625           7893           5032          40256
             0.01        170.201      0.0769316           5607           4924          39389
             0.01        143.318      0.0785391           5178           5026          40212
             0.01        116.285      0.0775488           5964           4964          39705
             0.01        129.784      0.0780156           5966           4990          39944
             0.01        111.944      0.0783867           5535           5015          40134
            0.011        222.748       0.083377           8448           5337          42689
            0.011        196.642       0.084457           8079           5405          43242
            0.011        227.388      0.0825781           6355           5286          42280
            0.011        190.764      0.0830566           8969           5318          42525
            0.011        188.574      0.0837305           8761           5360          42870
            0.011        210.668      0.0841895           9136           5388          43105
            0.011        191.252      0.0850278           6199           5356          42854
            0.011        182.485      0.0817793           7351           5234          41871
            0.011        226.742      0.0839785           6779           5374          42997
            0.011        211.889      0.0820117           5741           5249          41990
            0.011        189.444      0.0842656           6424           5393          43144
            0.011        156.955      0.0832187           5286           5328          42608
            0.011        204.922      0.0834082           7860           5342          42705
            0.011        249.715      0.0836367           6727           5352          42822
            0.011        223.321      0.0851074           5540           5452          43575
            0.011        190.774       0.085248           5695           5459          43647
            0.011        211.574      0.0830547           7216           5314          42524
            0.011        201.916        0.08225           7446           5262          42112
            0.011         195.36      0.0846113           4434           5415          43321
            0.011        199.723      0.0838301           8606           5365          42921
            0.012        280.121      0.0867773           7036           5554          44430
            0.012        242.268      0.0878203           7794           5622          44964
            0.012        238.144      0.0871875           6555           5579          44640
            0.012        302.459      0.0886953           8305           5680          45412
            0.012        275.052      0.0882988           7321           5654          45209
            0.012        227.102      0.0866738           9188           5546          44377
            0.012        262.428       0.087793           6734           5620          44950
            0.012        241.226      0.0886543           8037           5674          45391
            0.012         267.68      0.0873535           7643           5590          44725
            0.012        279.515      0.0865176           9369           5539          44297
            0.012        246.451      0.0874238           8180           5595          44761
            0.012        269.999      0.0891582           7813           5704          45649
            0.012        235.978      0.0882422           7448           5649          45180
            0.012        259.936      0.0883945           8121           5657          45258
            0.012         255.46      0.0879922           7892           5632          45052
            0.012        244.141      0.0865215           8673           5540          44299
            0.012        255.179      0.0881367           7167           5637          45126
            0.012        270.203      0.0875254           7009           5603          44813
            0.012        180.188      0.0885313           5910           5664          45328
            0.012        253.208      0.0890605           8355           5699          45599
            0.013         313.12      0.0958851           6302           5944          47559
            0.013        317.642      0.0935664           9034           5989          47906
            0.013        345.685      0.0933477           8139           5973          47794
            0.013        323.902      0.0927676           7907           5934          47497
            0.013         304.49      0.0932956           7637           5878          47021
            0.013        331.757      0.0924668           8515           5917          47343
            0.013        299.401      0.0921484           8479           5898          47180
            0.013        297.792      0.0938413           7396           5913          47296
            0.013        302.567      0.0957258           7528           5937          47480
            0.013        332.376       0.093998           7144           5919          47375
            0.013        319.509      0.0921562           8017           5899          47184
            0.013        320.288      0.0939484           7748           5917          47350
            0.013        322.778      0.0932559           7728           5971          47747
            0.013         312.01      0.0956048           7964           5924          47420
            0.013        322.622      0.0918887           7280           5882          47047
            0.013        321.165      0.0930957           7648           5958          47665
            0.013        380.321      0.0922598           8292           5906          47237
            0.013        294.074       0.094748           6886           5876          46995
            0.013         304.96       0.095623           7801           6025          48194
            0.013        320.934      0.0938867           8290           6007          48070
            0.014        410.032      0.0968909           8695           6104          48833
            0.014        368.745      0.0994839           6937           6168          49344
            0.014        431.318      0.0983306           8103           6094          48772
            0.014        413.413      0.0990726           7649           6142          49140
            0.014        443.877       0.098129           7887           6184          49457
            0.014         411.96       0.100012           8368           6197          49606
            0.014        343.792       0.095543           7985           6116          48918
            0.014        396.759      0.0967143           8108           6097          48744
            0.014        382.693        0.10067           7918           6139          49127
            0.014        414.992       0.100814           7344           6146          49197
            0.014        406.457      0.0964688           7745           6174          49392
            0.014        355.799      0.0970933           9213           6120          48935
            0.014          450.3      0.0964464           8696           6075          48609
            0.014        430.693      0.0956071           8164           6021          48186
            0.014        388.034       0.098246           7741           6090          48730
            0.014        391.641       0.100244           8572           6115          48919
            0.014        365.684      0.0984597           8674           6104          48836
            0.014        408.246      0.0960099           8364           6049          48389
            0.014        414.643      0.0970806           8276           6020          48152
            0.014        377.252      0.0976944           7494           6154          49238
            0.015        475.212      0.0997718           8763           6284          50285
            0.015        531.785       0.103744           8569           6329          50627
            0.015        478.492       0.101885           7535           6316          50535
            0.015         467.74       0.102022           8277           6427          51419
            0.015        486.622       0.102183           7850           6332          50683
            0.015        530.521       0.105469           8385           6434          51469
            0.015        489.714       0.102748           7686           6367          50963
            0.015        418.444       0.104764           8324           6393          51125
            0.015        462.426        0.10432           8590           6362          50908
            0.015        471.508         0.1064           9161           6489          51923
            0.015        472.392        0.10273           7927           6370          50954
            0.015        435.886       0.103323           8397           6405          51248
            0.015        456.316       0.102956           8463           6384          51066
            0.015        537.989       0.103313           9229           6405          51243
            0.015         482.44       0.103506           8823           6315          50511
            0.015        522.995       0.104523           8398           6374          51007
            0.015        468.487       0.105281           8346           6421          51377
            0.015        461.258       0.102177           8764           6336          50680
            0.015        497.779        0.10392           7494           6339          50713
            0.015        473.993      0.0996523           8769           6379          51022
            0.016        516.364       0.108301           8874           6604          52851
            0.016         497.64       0.105746           8438           6450          51604
            0.016        548.429       0.108894           8047           6535          52269
            0.016        539.797       0.110702           8609           6643          53137
            0.016        545.668       0.107107           8778           6533          52268
            0.016        478.045        0.10549           7991           6437          51479
            0.016        579.359       0.107174           8489           6538          52301
            0.016        487.829       0.106805           7889           6513          52121
            0.016         552.59       0.108408           7606           6612          52903
            0.016        549.123       0.104738           7971           6493          51950
            0.016        512.433       0.105895           8846           6459          51677
            0.016        546.432       0.109556           9484           6571          52587
            0.016        513.528       0.108577           8640           6515          52117
            0.016        610.197       0.107801           9514           6576          52607
            0.016        535.407       0.108371           8492           6610          52885
            0.016        540.537       0.106775           8003           6511          52106
            0.016        523.408       0.108379           7899           6608          52889
            0.016        545.054        0.10402           7970           6553          52426
            0.016        515.824       0.106111           8854           6579          52631
            0.016        563.704       0.105814           7488           6457          51637
];

rows = size(data, 1);
cols = size(data, 2);

data_delay = [];
for i = 1:rows/20,
   ifirst = (i - 1) * 20 + 1;
   ilast  = ifirst + 20 - 1;
   tmp = data(ifirst:ilast, cols-5+1);
   avg = mean(tmp);
   [h sig ci] = ttest(tmp, 0.1);
   ci = (ci(2)-ci(1))/2;
   data_delay = [data_delay; data(ifirst, 1:cols-5), avg ci];
end

figure(1);
hold on;
plot(data_delay(:,1), data_delay(:,2), symbol);

data_throughput = [];
for i = 1:rows/20,
   ifirst = (i - 1) * 20 + 1;
   ilast  = ifirst + 20 - 1;
   tmp = data(ifirst:ilast, cols-5+2);
   avg = mean(tmp);
   [h sig ci] = ttest(tmp, 0.1);
   ci = (ci(2)-ci(1))/2;
   data_throughput = [data_throughput; data(ifirst, 1:cols-5), avg ci];
end

figure(2);
hold on;
plot(data_throughput(:,1), data_throughput(:,2), symbol);

data_maxdelay = [];
for i = 1:rows/20,
   ifirst = (i - 1) * 20 + 1;
   ilast  = ifirst + 20 - 1;
   tmp = data(ifirst:ilast, cols-5+3);
   avg = mean(tmp);
   [h sig ci] = ttest(tmp, 0.1);
   ci = (ci(2)-ci(1))/2;
   data_maxdelay = [data_maxdelay; data(ifirst, 1:cols-5), avg ci];
end

figure(3);
hold on;
plot(data_maxdelay(:,1), data_maxdelay(:,2), symbol);


%-------- Saturation Analysis -----------
slope=[];
for i=2:size(data_throughput,1),
    slope(i-1) = (data_throughput(i,2)-data_throughput(i-1,2))/(data_throughput(i,1)-data_throughput(i-1,1));
end

for i=2:size(slope,2),
    if slope(i) < (0.95*mean(slope(1:i)))
        max_pir = data_throughput(i, 1);
        max_throughput = data_throughput(i, 2);
        min_delay = data_delay(i, 2);
        break;
    end
end
