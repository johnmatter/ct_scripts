#include <map>
#include <vector>
#include <iostream>
#include <TString.h>

// This will format TDC cuts to be used in your hcana replay directory, PARAM/TRIG/trig.param
// The order must be maintained, as hcana makes assumptions about the order of some of these.
// Apologies to anyone offended by my three-assignments-per-line nonsense with weird alignment.
void format_tdc_cuts() {
    //-------------------------------------------------------------------------------------------------------------------------
    // text file to save to
    TString csvFilename = "/home/jmatter/ct_scripts/analysis/tdc_window/tdc_cuts.txt";

    std::vector<TString> tdcNames;
    std::map<TString, Int_t> tdcMin;
    std::map<TString, Int_t> tdcMax;

    tdcNames.push_back("h1X");             tdcMin["h1X"] =             0;     tdcMax ["h1X"] =             100000;
    tdcNames.push_back("h1Y");             tdcMin["h1Y"] =             0;     tdcMax ["h1Y"] =             100000;
    tdcNames.push_back("h2X");             tdcMin["h2X"] =             0;     tdcMax ["h2X"] =             100000;
    tdcNames.push_back("h2Y");             tdcMin["h2Y"] =             0;     tdcMax ["h2Y"] =             100000;
    tdcNames.push_back("h1T");             tdcMin["h1T"] =             0;     tdcMax ["h1T"] =             100000;
    tdcNames.push_back("h2T");             tdcMin["h2T"] =             0;     tdcMax ["h2T"] =             100000;
    tdcNames.push_back("hT1");             tdcMin["hT1"] =             1500;  tdcMax ["hT1"] =             100000;

    tdcNames.push_back("hASUM");           tdcMin["hASUM"] =           0;     tdcMax ["hASUM"] =           100000;
    tdcNames.push_back("hBSUM");           tdcMin["hBSUM"] =           0;     tdcMax ["hBSUM"] =           100000;
    tdcNames.push_back("hCSUM");           tdcMin["hCSUM"] =           0;     tdcMax ["hCSUM"] =           100000;
    tdcNames.push_back("hDSUM");           tdcMin["hDSUM"] =           0;     tdcMax ["hDSUM"] =           100000;
    tdcNames.push_back("hPRLO");           tdcMin["hPRLO"] =           0;     tdcMax ["hPRLO"] =           100000;
    tdcNames.push_back("hPRHI");           tdcMin["hPRHI"] =           0;     tdcMax ["hPRHI"] =           100000;
    tdcNames.push_back("hSHWR");           tdcMin["hSHWR"] =           0;     tdcMax ["hSHWR"] =           100000;
    tdcNames.push_back("hEDTM");           tdcMin["hEDTM"] =           1530;  tdcMax ["hEDTM"] =           1860;
    tdcNames.push_back("hCER");            tdcMin["hCER"] =            0;     tdcMax ["hCER"] =            100000;
    tdcNames.push_back("hT2");             tdcMin["hT2"] =             1500;  tdcMax ["hT2"] =             4500;

    tdcNames.push_back("hDCREF1");         tdcMin["hDCREF1"] =         0;     tdcMax ["hDCREF1"] =         100000;
    tdcNames.push_back("hDCREF2");         tdcMin["hDCREF2"] =         0;     tdcMax ["hDCREF2"] =         100000;
    tdcNames.push_back("hDCREF3");         tdcMin["hDCREF3"] =         0;     tdcMax ["hDCREF3"] =         100000;
    tdcNames.push_back("hDCREF4");         tdcMin["hDCREF4"] =         0;     tdcMax ["hDCREF4"] =         100000;

    tdcNames.push_back("hTRIG1_ROC1");     tdcMin["hTRIG1_ROC1"] =     0;     tdcMax ["hTRIG1_ROC1"] =     100000;
    tdcNames.push_back("hTRIG2_ROC1");     tdcMin["hTRIG2_ROC1"] =     0;     tdcMax ["hTRIG2_ROC1"] =     100000;
    tdcNames.push_back("hTRIG3_ROC1");     tdcMin["hTRIG3_ROC1"] =     0;     tdcMax ["hTRIG3_ROC1"] =     100000;
    tdcNames.push_back("hTRIG4_ROC1");     tdcMin["hTRIG4_ROC1"] =     0;     tdcMax ["hTRIG4_ROC1"] =     100000;
    tdcNames.push_back("hTRIG5_ROC1");     tdcMin["hTRIG5_ROC1"] =     0;     tdcMax ["hTRIG5_ROC1"] =     100000;
    tdcNames.push_back("hTRIG6_ROC1");     tdcMin["hTRIG6_ROC1"] =     0;     tdcMax ["hTRIG6_ROC1"] =     100000;
    tdcNames.push_back("pTRIG1_ROC1");     tdcMin["pTRIG1_ROC1"] =     2300;  tdcMax ["pTRIG1_ROC1"] =     3600;
    tdcNames.push_back("pTRIG2_ROC1");     tdcMin["pTRIG2_ROC1"] =     0;     tdcMax ["pTRIG2_ROC1"] =     100000;
    tdcNames.push_back("pTRIG3_ROC1");     tdcMin["pTRIG3_ROC1"] =     0;     tdcMax ["pTRIG3_ROC1"] =     100000;
    tdcNames.push_back("pTRIG4_ROC1");     tdcMin["pTRIG4_ROC1"] =     2400;  tdcMax ["pTRIG4_ROC1"] =     3500;
    tdcNames.push_back("pTRIG5_ROC1");     tdcMin["pTRIG5_ROC1"] =     0;     tdcMax ["pTRIG5_ROC1"] =     100000;
    tdcNames.push_back("pTRIG6_ROC1");     tdcMin["pTRIG6_ROC1"] =     2700;  tdcMax ["pTRIG6_ROC1"] =     3030;

    tdcNames.push_back("pT1");             tdcMin["pT1"] =             3000;  tdcMax ["pT1"] =             4500;
    tdcNames.push_back("pT2");             tdcMin["pT2"] =             2000;  tdcMax ["pT2"] =             100000;
    tdcNames.push_back("p1X");             tdcMin["p1X"] =             0;     tdcMax ["p1X"] =             100000;
    tdcNames.push_back("p1Y");             tdcMin["p1Y"] =             0;     tdcMax ["p1Y"] =             100000;
    tdcNames.push_back("p2X");             tdcMin["p2X"] =             0;     tdcMax ["p2X"] =             100000;
    tdcNames.push_back("p2Y");             tdcMin["p2Y"] =             0;     tdcMax ["p2Y"] =             100000;
    tdcNames.push_back("p1T");             tdcMin["p1T"] =             0;     tdcMax ["p1T"] =             100000;
    tdcNames.push_back("p2T");             tdcMin["p2T"] =             0;     tdcMax ["p2T"] =             100000;
    tdcNames.push_back("pT3");             tdcMin["pT3"] =             0;     tdcMax ["pT3"] =             100000;

    tdcNames.push_back("pAER");            tdcMin["pAER"] =            0;     tdcMax ["pAER"] =            100000;
    tdcNames.push_back("pHGCER");          tdcMin["pHGCER"] =          0;     tdcMax ["pHGCER"] =          100000;
    tdcNames.push_back("pNGCER");          tdcMin["pNGCER"] =          0;     tdcMax ["pNGCER"] =          100000;
    tdcNames.push_back("pDCREF1");         tdcMin["pDCREF1"] =         12500; tdcMax ["pDCREF1"] =         100000;
    tdcNames.push_back("pDCREF2");         tdcMin["pDCREF2"] =         12500; tdcMax ["pDCREF2"] =         100000;
    tdcNames.push_back("pDCREF3");         tdcMin["pDCREF3"] =         12500; tdcMax ["pDCREF3"] =         100000;
    tdcNames.push_back("pDCREF4");         tdcMin["pDCREF4"] =         12500; tdcMax ["pDCREF4"] =         100000;
    tdcNames.push_back("pDCREF5");         tdcMin["pDCREF5"] =         12500; tdcMax ["pDCREF5"] =         100000;
    tdcNames.push_back("pDCREF6");         tdcMin["pDCREF6"] =         12500; tdcMax ["pDCREF6"] =         100000;
    tdcNames.push_back("pDCREF7");         tdcMin["pDCREF7"] =         12500; tdcMax ["pDCREF7"] =         100000;
    tdcNames.push_back("pDCREF8");         tdcMin["pDCREF8"] =         12500; tdcMax ["pDCREF8"] =         100000;
    tdcNames.push_back("pDCREF9");         tdcMin["pDCREF9"] =         12500; tdcMax ["pDCREF9"] =         100000;
    tdcNames.push_back("pDCREF10");        tdcMin["pDCREF10"] =        12500; tdcMax ["pDCREF10"] =        100000;
    tdcNames.push_back("pEDTM");           tdcMin["pEDTM"] =           1530;  tdcMax ["pEDTM"] =           1860;
    tdcNames.push_back("pPRLO");           tdcMin["pPRLO"] =           0;     tdcMax ["pPRLO"] =           100000;
    tdcNames.push_back("pPRHI");           tdcMin["pPRHI"] =           0;     tdcMax ["pPRHI"] =           100000;

    tdcNames.push_back("pTRIG1_ROC2");     tdcMin["pTRIG1_ROC2"] =     2700;  tdcMax ["pTRIG1_ROC2"] =     4000;
    tdcNames.push_back("pTRIG2_ROC2");     tdcMin["pTRIG2_ROC2"] =     0;     tdcMax ["pTRIG2_ROC2"] =     100000;
    tdcNames.push_back("pTRIG3_ROC2");     tdcMin["pTRIG3_ROC2"] =     0;     tdcMax ["pTRIG3_ROC2"] =     100000;
    tdcNames.push_back("pTRIG4_ROC2");     tdcMin["pTRIG4_ROC2"] =     3100;  tdcMax ["pTRIG4_ROC2"] =     4000;
    tdcNames.push_back("pTRIG5_ROC2");     tdcMin["pTRIG5_ROC2"] =     0;     tdcMax ["pTRIG5_ROC2"] =     100000;
    tdcNames.push_back("pTRIG6_ROC2");     tdcMin["pTRIG6_ROC2"] =     3400;  tdcMax ["pTRIG6_ROC2"] =     3720;
    tdcNames.push_back("hTRIG1_ROC2");     tdcMin["hTRIG1_ROC2"] =     0;     tdcMax ["hTRIG1_ROC2"] =     100000;
    tdcNames.push_back("hTRIG2_ROC2");     tdcMin["hTRIG2_ROC2"] =     0;     tdcMax ["hTRIG2_ROC2"] =     100000;
    tdcNames.push_back("hTRIG3_ROC2");     tdcMin["hTRIG3_ROC2"] =     0;     tdcMax ["hTRIG3_ROC2"] =     100000;
    tdcNames.push_back("hTRIG4_ROC2");     tdcMin["hTRIG4_ROC2"] =     0;     tdcMax ["hTRIG4_ROC2"] =     100000;
    tdcNames.push_back("hTRIG5_ROC2");     tdcMin["hTRIG5_ROC2"] =     0;     tdcMax ["hTRIG5_ROC2"] =     100000;
    tdcNames.push_back("hTRIG6_ROC2");     tdcMin["hTRIG6_ROC2"] =     0;     tdcMax ["hTRIG6_ROC2"] =     100000;

    tdcNames.push_back("pSTOF_ROC2");      tdcMin["pSTOF_ROC2"] =      0;     tdcMax ["pSTOF_ROC2"] =      100000;
    tdcNames.push_back("pEL_LO_LO_ROC2");  tdcMin["pEL_LO_LO_ROC2"] =  0;     tdcMax ["pEL_LO_LO_ROC2"] =  100000;
    tdcNames.push_back("pEL_LO_ROC2");     tdcMin["pEL_LO_ROC2"] =     0;     tdcMax ["pEL_LO_ROC2"] =     100000;
    tdcNames.push_back("pEL_HI_ROC2");     tdcMin["pEL_HI_ROC2"] =     0;     tdcMax ["pEL_HI_ROC2"] =     100000;
    tdcNames.push_back("pEL_REAL_ROC2");   tdcMin["pEL_REAL_ROC2"] =   0;     tdcMax ["pEL_REAL_ROC2"] =   100000;
    tdcNames.push_back("pEL_CLEAN_ROC2");  tdcMin["pEL_CLEAN_ROC2"] =  0;     tdcMax ["pEL_CLEAN_ROC2"] =  100000;
    tdcNames.push_back("hSTOF_ROC2");      tdcMin["hSTOF_ROC2"] =      0;     tdcMax ["hSTOF_ROC2"] =      100000;
    tdcNames.push_back("hEL_LO_LO_ROC2");  tdcMin["hEL_LO_LO_ROC2"] =  0;     tdcMax ["hEL_LO_LO_ROC2"] =  100000;
    tdcNames.push_back("hEL_LO_ROC2");     tdcMin["hEL_LO_ROC2"] =     0;     tdcMax ["hEL_LO_ROC2"] =     100000;
    tdcNames.push_back("hEL_HI_ROC2");     tdcMin["hEL_HI_ROC2"] =     0;     tdcMax ["hEL_HI_ROC2"] =     100000;
    tdcNames.push_back("hEL_REAL_ROC2");   tdcMin["hEL_REAL_ROC2"] =   0;     tdcMax ["hEL_REAL_ROC2"] =   100000;
    tdcNames.push_back("hEL_CLEAN_ROC2");  tdcMin["hEL_CLEAN_ROC2"] =  0;     tdcMax ["hEL_CLEAN_ROC2"] =  100000;
    tdcNames.push_back("pSTOF_ROC1");      tdcMin["pSTOF_ROC1"] =      0;     tdcMax ["pSTOF_ROC1"] =      100000;
    tdcNames.push_back("pEL_LO_LO_ROC1");  tdcMin["pEL_LO_LO_ROC1"] =  0;     tdcMax ["pEL_LO_LO_ROC1"] =  100000;
    tdcNames.push_back("pEL_LO_ROC1");     tdcMin["pEL_LO_ROC1"] =     0;     tdcMax ["pEL_LO_ROC1"] =     100000;
    tdcNames.push_back("pEL_HI_ROC1");     tdcMin["pEL_HI_ROC1"] =     0;     tdcMax ["pEL_HI_ROC1"] =     100000;
    tdcNames.push_back("pEL_REAL_ROC1");   tdcMin["pEL_REAL_ROC1"] =   0;     tdcMax ["pEL_REAL_ROC1"] =   100000;
    tdcNames.push_back("pEL_CLEAN_ROC1");  tdcMin["pEL_CLEAN_ROC1"] =  0;     tdcMax ["pEL_CLEAN_ROC1"] =  100000;
    tdcNames.push_back("hSTOF_ROC1");      tdcMin["hSTOF_ROC1"] =      0;     tdcMax ["hSTOF_ROC1"] =      100000;
    tdcNames.push_back("hEL_LO_LO_ROC1");  tdcMin["hEL_LO_LO_ROC1"] =  0;     tdcMax ["hEL_LO_LO_ROC1"] =  100000;
    tdcNames.push_back("hEL_LO_ROC1");     tdcMin["hEL_LO_ROC1"] =     0;     tdcMax ["hEL_LO_ROC1"] =     100000;
    tdcNames.push_back("hEL_HI_ROC1");     tdcMin["hEL_HI_ROC1"] =     0;     tdcMax ["hEL_HI_ROC1"] =     100000;
    tdcNames.push_back("hEL_REAL_ROC1");   tdcMin["hEL_REAL_ROC1"] =   0;     tdcMax ["hEL_REAL_ROC1"] =   100000;
    tdcNames.push_back("hEL_CLEAN_ROC1");  tdcMin["hEL_CLEAN_ROC1"] =  0;     tdcMax ["hEL_CLEAN_ROC1"] =  100000;

    tdcNames.push_back("pPRE40_ROC1");     tdcMin["pPRE40_ROC1"] =     0;     tdcMax ["pPRE40_ROC1"] =     100000;
    tdcNames.push_back("pPRE100_ROC1");    tdcMin["pPRE100_ROC1"] =    0;     tdcMax ["pPRE100_ROC1"] =    100000;
    tdcNames.push_back("pPRE150_ROC1");    tdcMin["pPRE150_ROC1"] =    0;     tdcMax ["pPRE150_ROC1"] =    100000;
    tdcNames.push_back("pPRE200_ROC1");    tdcMin["pPRE200_ROC1"] =    0;     tdcMax ["pPRE200_ROC1"] =    100000;
    tdcNames.push_back("hPRE40_ROC1");     tdcMin["hPRE40_ROC1"] =     0;     tdcMax ["hPRE40_ROC1"] =     100000;
    tdcNames.push_back("hPRE100_ROC1");    tdcMin["hPRE100_ROC1"] =    0;     tdcMax ["hPRE100_ROC1"] =    100000;
    tdcNames.push_back("hPRE150_ROC1");    tdcMin["hPRE150_ROC1"] =    0;     tdcMax ["hPRE150_ROC1"] =    100000;
    tdcNames.push_back("hPRE200_ROC1");    tdcMin["hPRE200_ROC1"] =    0;     tdcMax ["hPRE200_ROC1"] =    100000;
    tdcNames.push_back("pPRE40_ROC2");     tdcMin["pPRE40_ROC2"] =     0;     tdcMax ["pPRE40_ROC2"] =     100000;
    tdcNames.push_back("pPRE100_ROC2");    tdcMin["pPRE100_ROC2"] =    0;     tdcMax ["pPRE100_ROC2"] =    100000;
    tdcNames.push_back("pPRE150_ROC2");    tdcMin["pPRE150_ROC2"] =    0;     tdcMax ["pPRE150_ROC2"] =    100000;
    tdcNames.push_back("pPRE200_ROC2");    tdcMin["pPRE200_ROC2"] =    0;     tdcMax ["pPRE200_ROC2"] =    100000;
    tdcNames.push_back("hPRE40_ROC2");     tdcMin["hPRE40_ROC2"] =     0;     tdcMax ["hPRE40_ROC2"] =     100000;
    tdcNames.push_back("hPRE100_ROC2");    tdcMin["hPRE100_ROC2"] =    0;     tdcMax ["hPRE100_ROC2"] =    100000;
    tdcNames.push_back("hPRE150_ROC2");    tdcMin["hPRE150_ROC2"] =    0;     tdcMax ["hPRE150_ROC2"] =    100000;
    tdcNames.push_back("hPRE200_ROC2");    tdcMin["hPRE200_ROC2"] =    0;     tdcMax ["hPRE200_ROC2"] =    100000;
    tdcNames.push_back("hDCREF5");         tdcMin["hDCREF5"] =         0;     tdcMax ["hDCREF5"] =         100000;
    tdcNames.push_back("hT3");             tdcMin["hT3"] =             0;     tdcMax ["hT3"] =             100000;
    tdcNames.push_back("pRF");             tdcMin["pRF"] =             0;     tdcMax ["pRF"] =             100000;
    tdcNames.push_back("hHODO_RF");        tdcMin["hHODO_RF"] =        0;     tdcMax ["hHODO_RF"] =        100000;
    tdcNames.push_back("pHODO_RF");        tdcMin["pHODO_RF"] =        0;     tdcMax ["pHODO_RF"] =        100000;


    // --------------------------------------------------
    // Print

    // // Open file
    // std::ofstream ofs;
    // ofs.open(csvFilename.Data());

    int tdcsPerLine = 5;

    // Names
    std::cout << "t_coin_tdcNames = \"";
    for (int i=0; i<tdcNames.size(); i++) {
        // Print name
        std::cout << tdcNames[i];

        // Print space if this isn't the last element
        if((i+1) < tdcNames.size()) {
            std::cout << " ";

            // newline to break up the text
            if((i+1)%tdcsPerLine==0) {
                std::cout << std::endl;
            }
        }
    };
    std::cout << "\"" << std::endl;
    std::cout << std::endl;

    std::cout << "t_coin_TdcTimeWindowMin = ";
    for (int i=0; i<tdcNames.size(); i++) {
        // Print name
        std::cout << tdcMin[tdcNames[i]];

        // Print comma if this isn't the last element
        if((i+1) < tdcNames.size()) {
            std::cout << ", ";

            // newline to break up the text
            if((i+1)%tdcsPerLine==0) {
                std::cout << std::endl;
            }
        }
    };
    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "t_coin_TdcTimeWindowMax = \"";
    for (int i=0; i<tdcNames.size(); i++) {
        // Print name
        std::cout << tdcMax[tdcNames[i]];

        // Print comma if this isn't the last element
        if((i+1) < tdcNames.size()) {
            std::cout << ", ";

            // newline to break up the text
            if((i+1)%tdcsPerLine==0) {
                std::cout << std::endl;
            }
        }
    };
    std::cout << std::endl;

}
