% fname: routing_xy__topology_8x8__.m
% ./noxim -routing xy -dimx 8 -dimy 8  -sim 10000 -warmup 2000 -size 8 8 -buffer 8 

function [max_pir, max_throughput, min_delay] = routing_xy__topology_8x8__(symbol)

data = [
%             pir      avg_delay     throughput      max_delay       rpackets         rflits
             0.01        23.4943      0.0809043            156           5173          41423
             0.01        23.0426      0.0786035            129           5028          40245
             0.01        23.9873      0.0797949            157           5105          40855
             0.01        23.0435       0.080168            146           5131          41046
             0.01        23.6385      0.0813242            158           5203          41638
             0.01        23.4636      0.0813809            152           5213          41667
             0.01        23.4254      0.0808926            145           5179          41417
             0.01        22.7758      0.0791191            117           5068          40509
             0.01           23.4      0.0804297            191           5147          41180
             0.01        22.7836      0.0802305            131           5133          41078
             0.01        23.2334      0.0800098            130           5119          40965
             0.01        23.6294      0.0816719            150           5227          41816
             0.01        22.1969      0.0791309            129           5063          40515
             0.01        23.8348      0.0810488            138           5187          41497
             0.01        23.1343      0.0794512            209           5087          40679
             0.01        23.4849      0.0813965            142           5207          41675
             0.01        22.2943      0.0786113            108           5032          40249
             0.01        23.9857      0.0817754            136           5235          41869
             0.01        23.0271      0.0813359            133           5208          41644
             0.01        23.7096      0.0812617            160           5199          41606
            0.011        25.6591       0.088873            164           5690          45503
            0.011        25.6094      0.0901426            178           5768          46153
            0.011        24.7761      0.0865605            153           5537          44319
            0.011        25.3401      0.0881875            149           5645          45152
            0.011        24.9383      0.0891719            139           5708          45656
            0.011        24.4125       0.087166            157           5580          44629
            0.011          24.53       0.088127            129           5638          45121
            0.011        25.6221      0.0895195            139           5729          45834
            0.011        25.1626      0.0884063            202           5658          45264
            0.011         25.254       0.087877            150           5622          44993
            0.011        25.5166      0.0882422            133           5650          45180
            0.011        24.9628      0.0865078            167           5536          44292
            0.011        24.9427      0.0874121            163           5598          44755
            0.011         25.578      0.0891055            170           5702          45622
            0.011        24.8441      0.0883672            161           5657          45244
            0.011        26.0472      0.0878125            156           5619          44960
            0.011        25.4017      0.0887891            146           5688          45460
            0.011        25.2602      0.0877852            169           5619          44946
            0.011        25.0319      0.0858027            148           5490          43931
            0.011        25.6964       0.088877            155           5686          45505
            0.012        26.9479       0.093998            191           6013          48127
            0.012         28.278      0.0975254            230           6241          49933
            0.012         27.694      0.0959805            162           6141          49142
            0.012        28.2045      0.0974883            224           6244          49914
            0.012        27.5627      0.0957051            188           6126          49001
            0.012        27.5595      0.0957441            206           6127          49021
            0.012        26.8219      0.0932676            182           5968          47753
            0.012         27.435      0.0945605            181           6053          48415
            0.012        27.0138      0.0961406            180           6155          49224
            0.012        26.7062      0.0954492            157           6110          48870
            0.012        27.8212      0.0965801            215           6180          49449
            0.012        27.3264      0.0963203            157           6161          49316
            0.012        27.9489      0.0960508            262           6146          49178
            0.012        27.7697      0.0977441            219           6257          50045
            0.012        26.6538       0.093666            202           5994          47957
            0.012        28.2369         0.0975            171           6242          49920
            0.012        26.9676      0.0948789            159           6071          48578
            0.012        26.9134      0.0942109            159           6030          48236
            0.012        26.9812      0.0954785            184           6107          48885
            0.012        26.6954      0.0976152            179           6248          49979
            0.013        31.7586       0.104104            263           6662          53301
            0.013        30.5538       0.104158            355           6669          53329
            0.013         30.151        0.10433            175           6680          53417
            0.013        29.1337       0.102439            267           6558          52449
            0.013        30.7211       0.101227            194           6480          51828
            0.013        30.4994       0.104617            186           6698          53564
            0.013        30.6399       0.105479            212           6751          54005
            0.013        30.2403       0.102678            174           6571          52571
            0.013        28.7311       0.101781            167           6512          52112
            0.013        29.8562       0.104959            218           6720          53739
            0.013        29.7247       0.103268            174           6610          52873
            0.013        30.6587       0.105078            345           6727          53800
            0.013        29.8316       0.104967            223           6716          53743
            0.013        30.1544        0.10465            242           6697          53581
            0.013        31.8112       0.106457            238           6810          54506
            0.013        29.3427       0.102986            181           6588          52729
            0.013         31.417       0.103766            324           6640          53128
            0.013        30.8058       0.103746            268           6641          53118
            0.013        31.3417          0.106            284           6781          54272
            0.013        31.4952       0.102586            321           6567          52524
            0.014        33.9613       0.110559            235           7076          56606
            0.014        34.8064       0.112762            209           7216          57734
            0.014        33.8638       0.113455            185           7261          58089
            0.014        33.5676       0.112025            255           7172          57357
            0.014        31.6022        0.11075            180           7089          56704
            0.014        34.1288       0.112316            242           7192          57506
            0.014        34.1475       0.112441            236           7195          57570
            0.014        34.0065       0.113187            263           7245          57952
            0.014        34.1697       0.112412            365           7193          57555
            0.014        32.9361       0.111781            262           7154          57232
            0.014        32.9087       0.111437            203           7132          57056
            0.014        33.2911       0.110031            359           7039          56336
            0.014        33.8179       0.110564            231           7077          56609
            0.014        33.1263       0.110893            254           7096          56777
            0.014        35.6019       0.113764            297           7282          58247
            0.014        34.1051        0.11283            320           7220          57769
            0.014        34.0946       0.110881            250           7096          56771
            0.014        35.5061       0.112053            431           7172          57371
            0.014        34.0191       0.113436            259           7260          58079
            0.014        33.0847       0.110707            240           7084          56682
            0.015         39.941       0.122912            320           7867          62931
            0.015        37.8391       0.120965            259           7742          61934
            0.015        39.0823       0.120564            400           7724          61729
            0.015        38.3158       0.118004            317           7553          60418
            0.015        37.3378       0.119955            322           7676          61417
            0.015        38.6474       0.119172            366           7623          61016
            0.015        41.4921       0.121195            365           7758          62052
            0.015        37.0895       0.120053            277           7683          61467
            0.015        39.3801       0.122557            246           7840          62749
            0.015        37.0223       0.120342            235           7699          61615
            0.015        36.4945       0.119988            284           7675          61434
            0.015        38.1619       0.120465            317           7709          61678
            0.015        38.3996       0.120955            349           7741          61929
            0.015        35.6422       0.119539            277           7650          61204
            0.015         37.122       0.118854            238           7606          60853
            0.015        36.4969         0.1195            290           7648          61184
            0.015        38.4209       0.120004            287           7682          61442
            0.015        38.4529        0.11915            385           7630          61005
            0.015        37.7171       0.118566            274           7587          60706
            0.015        38.3265       0.120365            316           7705          61627
            0.016        45.0782       0.128156            489           8200          65616
            0.016        48.1229       0.128734            450           8242          65912
            0.016        43.9337       0.129875            355           8313          66496
            0.016        40.3474       0.126678            260           8108          64859
            0.016        40.6914       0.126393            266           8089          64713
            0.016        44.7763       0.129268            360           8271          66185
            0.016        47.6838       0.129869            457           8314          66493
            0.016         42.803        0.12758            370           8163          65321
            0.016        47.0875        0.12824            348           8207          65659
            0.016        42.6715       0.130324            376           8340          66726
            0.016        46.6884       0.127004            499           8130          65026
            0.016        43.6576       0.126875            393           8117          64960
            0.016        45.5657       0.127934            465           8184          65502
            0.016        44.9949       0.126695            662           8110          64868
            0.016        44.4077       0.127312            325           8148          65184
            0.016        46.8459       0.129281            389           8270          66192
            0.016        45.1233        0.12891            362           8246          66002
            0.016        46.5848       0.128914            500           8251          66004
            0.016        42.9518       0.127297            366           8148          65176
            0.016        41.1278       0.127578            340           8168          65320
            0.017        49.0348       0.134539            327           8610          68884
            0.017        48.5089       0.133195            382           8521          68196
            0.017        50.6871       0.135746            386           8686          69502
            0.017        57.9748       0.136025            606           8707          69645
            0.017        53.9924       0.136133            397           8711          69700
            0.017        54.4286        0.13874            509           8881          71035
            0.017        53.8133       0.135982            485           8705          69623
            0.017        52.8812       0.134436            479           8601          68831
            0.017        52.8378       0.134299            474           8595          68761
            0.017         59.755       0.141027            578           9026          72206
            0.017         57.703       0.136025            599           8705          69645
            0.017        52.0631       0.135555            401           8678          69404
            0.017        55.8367       0.136393            609           8730          69833
            0.017         59.892       0.137699            575           8811          70502
            0.017        53.1659       0.136141            567           8708          69704
            0.017        52.2488       0.134646            513           8619          68939
            0.017        58.9987       0.137086            723           8770          70188
            0.017        52.4306        0.13607            377           8706          69668
            0.017        54.0932       0.136025            507           8706          69645
            0.017        55.1644         0.1375            515           8801          70400
            0.018        86.2397        0.14466           1222           9259          74066
            0.018        76.9397       0.146166            823           9354          74837
            0.018        56.9442        0.14268            470           9132          73052
            0.018        87.8393       0.145432            865           9308          74461
            0.018        79.5726         0.1425            989           9121          72960
            0.018        70.4403       0.145256            549           9300          74371
            0.018        68.0851        0.14393            560           9213          73692
            0.018        86.6029        0.14542           1202           9306          74455
            0.018        66.8156       0.142783            645           9142          73105
            0.018         84.915        0.14618            903           9354          74844
            0.018        75.8294       0.144498            906           9248          73983
            0.018         69.557       0.144594            993           9255          74032
            0.018        68.7866       0.145709            959           9322          74603
            0.018        68.0926       0.144711            586           9259          74092
            0.018         67.338       0.145568            679           9314          74531
            0.018        69.2035       0.145703            566           9324          74600
            0.018        97.8965        0.14609           1083           9352          74798
            0.018         63.682       0.142838            668           9141          73133
            0.018        66.9694        0.14299            792           9151          73211
            0.018        63.2629       0.141867            607           9084          72636
            0.019        112.373        0.15252           1317           9763          78090
            0.019        97.2644       0.153029           1002           9792          78351
            0.019        123.735       0.152453           1307           9757          78056
            0.019        118.805       0.150742           2238           9647          77180
            0.019        125.692       0.152328           1340           9747          77992
            0.019        126.229       0.153771           1229           9843          78731
            0.019        130.298       0.150746           1737           9642          77182
            0.019         119.91       0.151443           1221           9689          77539
            0.019        115.343       0.151961           1019           9725          77804
            0.019        134.477       0.152588           1275           9765          78125
            0.019        118.008       0.152068           1081           9734          77859
            0.019        118.636        0.15343           1138           9823          78556
            0.019        112.555       0.149338           1297           9560          76461
            0.019        142.693       0.151824           1846           9719          77734
            0.019        88.4282       0.150969           1355           9664          77296
            0.019        94.2101       0.154967            843           9918          79343
            0.019        128.903       0.153814            990           9845          78753
            0.019        99.7153       0.148061            874           9473          75807
            0.019        86.3561       0.151059            769           9665          77342
            0.019        108.533       0.152809           1040           9778          78238
             0.02        198.983       0.156375           1981          10009          80064
             0.02        182.102       0.156922           2835          10042          80344
             0.02        210.562       0.158121           3765          10124          80958
             0.02        166.369       0.156121           1463           9992          79934
             0.02        245.569       0.157553           2548          10084          80667
             0.02        145.246        0.15858           1292          10153          81193
             0.02        193.808       0.158262           1495          10129          81030
             0.02        165.405       0.156953           2131          10046          80360
             0.02        162.783       0.159629           1520          10217          81730
             0.02        219.693        0.15685           2328          10038          80307
             0.02        235.041       0.157939           2248          10108          80865
             0.02        181.249       0.157586           1603          10087          80684
             0.02        172.202       0.156818           1732          10034          80291
             0.02        163.832       0.156574           1862          10024          80166
             0.02        193.592       0.157855           2014          10106          80822
             0.02        212.516       0.156121           1835           9992          79934
             0.02         234.45       0.160162           1855          10248          82003
             0.02         231.23       0.157617           3051          10089          80700
             0.02        194.329       0.155109           1971           9924          79416
             0.02        201.089       0.157883           1816          10106          80836
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
