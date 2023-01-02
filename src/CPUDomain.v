module CPUDomain
  (input wire clock,
   input wire reset,
   input wire [31:0] in0_cpuDomainUARTIns_avl_jtaguart_readdata,
   input wire [0:0] in0_cpuDomainUARTIns_avl_jtaguart_waitrequest,
   input wire [0:0] in0_cpuDomainFromDRAM_canPeek,
   input wire [3:0] in0_cpuDomainFromDRAM_peek_dramRespBurstId,
   input wire [511:0] in0_cpuDomainFromDRAM_peek_dramRespData,
   input wire [15:0] in0_cpuDomainFromDRAM_peek_dramRespDataTagBits,
   input wire [0:0] out_cpuDomainToDRAM_consume_en,
   input wire [0:0] in0_cpuDomainFromSIMT_canPeek,
   input wire [31:0] in0_cpuDomainFromSIMT_peek,
   input wire [0:0] out_cpuDomainToSIMT_consume_en,
   output wire [0:0] in0_cpuDomainFromDRAM_consume_en,
   output wire [0:0] in0_cpuDomainFromSIMT_consume_en,
   output wire [0:0] out_cpuDomainUARTOuts_avl_jtaguart_read,
   output wire [0:0] out_cpuDomainUARTOuts_avl_jtaguart_write,
   output wire [31:0] out_cpuDomainUARTOuts_avl_jtaguart_writedata,
   output wire [2:0] out_cpuDomainUARTOuts_avl_jtaguart_address,
   output wire [0:0] out_cpuDomainToDRAM_canPeek,
   output wire [0:0] out_cpuDomainToDRAM_peek_dramReqIsStore,
   output wire [25:0] out_cpuDomainToDRAM_peek_dramReqAddr,
   output wire [511:0] out_cpuDomainToDRAM_peek_dramReqData,
   output wire [15:0] out_cpuDomainToDRAM_peek_dramReqDataTagBits,
   output wire [63:0] out_cpuDomainToDRAM_peek_dramReqByteEn,
   output wire [3:0] out_cpuDomainToDRAM_peek_dramReqBurst,
   output wire [0:0] out_cpuDomainToDRAM_peek_dramReqIsFinal,
   output wire [0:0] out_cpuDomainToSIMT_canPeek,
   output wire [1:0] out_cpuDomainToSIMT_peek_simtReqCmd_0,
   output wire [31:0] out_cpuDomainToSIMT_peek_simtReqAddr,
   output wire [31:0] out_cpuDomainToSIMT_peek_simtReqData);
  // Declarations
  //////////////////////////////////////////////////////////////////////////////
  wire [0:0] v_0;
  wire [0:0] v_1;
  wire [0:0] v_2;
  wire [31:0] v_3;
  wire [0:0] v_4;
  wire [0:0] v_5;
  wire [0:0] v_6;
  wire [0:0] v_7;
  wire [0:0] v_8;
  wire [0:0] v_9;
  wire [0:0] v_10;
  wire [0:0] v_11;
  wire [0:0] v_12;
  reg [0:0] v_13 = 1'h0;
  wire [0:0] v_14;
  wire [0:0] v_15;
  wire [0:0] v_16;
  wire [0:0] v_17;
  wire [0:0] v_18;
  wire [0:0] v_19;
  wire [0:0] v_20;
  wire [0:0] v_21;
  wire [0:0] v_22;
  wire [0:0] v_23;
  wire [0:0] v_24;
  wire [0:0] v_25;
  wire [0:0] v_26;
  wire [0:0] v_27;
  wire [0:0] v_28;
  wire [0:0] v_29;
  wire [0:0] v_30;
  wire [15:0] v_31;
  wire [0:0] v_32;
  wire [1:0] v_33;
  wire [1:0] v_34;
  reg [1:0] v_35 = 2'h0;
  wire [0:0] v_36;
  wire [0:0] v_37;
  wire [0:0] v_38;
  wire [0:0] act_39;
  wire [0:0] v_40;
  wire [0:0] v_41;
  wire [0:0] v_42;
  wire [0:0] v_43;
  wire [0:0] v_44;
  wire [0:0] v_45;
  wire [0:0] v_46;
  wire [0:0] v_47;
  wire [0:0] v_48;
  wire [0:0] v_49;
  wire [0:0] v_50;
  wire [0:0] v_51;
  wire [0:0] v_52;
  wire [0:0] v_53;
  reg [0:0] v_54 = 1'h0;
  wire [0:0] v_55;
  wire [7:0] v_56;
  wire [7:0] v_57;
  wire [7:0] v_58;
  reg [7:0] v_59 ;
  wire [0:0] v_60;
  wire [3:0] v_61;
  wire [511:0] v_62;
  wire [15:0] v_63;
  wire [0:0] v_64;
  wire [0:0] v_65;
  wire [1:0] v_66;
  wire [0:0] v_67;
  wire [2:0] v_68;
  wire [0:0] v_69;
  wire [4:0] v_70;
  wire [0:0] v_71;
  wire [0:0] v_72;
  wire [0:0] v_73;
  wire [0:0] v_74;
  wire [0:0] v_75;
  wire [0:0] v_76;
  wire [0:0] v_77;
  wire [31:0] v_78;
  wire [0:0] v_79;
  wire [31:0] v_80;
  wire [0:0] v_81;
  wire [0:0] v_82;
  wire [0:0] v_83;
  wire [0:0] v_84;
  wire [0:0] v_85;
  wire [0:0] v_86;
  wire [0:0] v_87;
  wire [0:0] v_88;
  wire [0:0] v_89;
  wire [0:0] v_90;
  wire [0:0] v_91;
  wire [0:0] v_92;
  wire [0:0] v_93;
  wire [0:0] v_94;
  wire [0:0] v_95;
  wire [0:0] v_96;
  wire [0:0] v_97;
  wire [0:0] vin0_consume_en_98;
  wire [0:0] vout_0_canPut_98;
  wire [0:0] vout_1_canPeek_98;
  wire [31:0] vout_1_peek_1_memRespData_98;
  wire [0:0] vout_1_peek_1_memRespDataTagBit_98;
  wire [0:0] vout_1_peek_1_memRespIsFinal_98;
  wire [0:0] vout_2_canPeek_98;
  wire [0:0] vout_2_peek_dramReqIsStore_98;
  wire [25:0] vout_2_peek_dramReqAddr_98;
  wire [511:0] vout_2_peek_dramReqData_98;
  wire [15:0] vout_2_peek_dramReqDataTagBits_98;
  wire [63:0] vout_2_peek_dramReqByteEn_98;
  wire [3:0] vout_2_peek_dramReqBurst_98;
  wire [0:0] vout_2_peek_dramReqIsFinal_98;
  wire [0:0] v_99;
  wire [31:0] v_100;
  wire [0:0] v_101;
  wire [0:0] v_102;
  wire [0:0] v_103;
  wire [0:0] v_104;
  wire [0:0] v_105;
  wire [0:0] v_106;
  wire [0:0] vin0_scalarUartIn_consume_en_107;
  wire [1:0] vin0_scalarMemReqs_put_0_1_memReqAccessWidth_107;
  wire [2:0] vin0_scalarMemReqs_put_0_1_memReqOp_107;
  wire [4:0] vin0_scalarMemReqs_put_0_1_memReqAMOInfo_amoOp_107;
  wire [0:0] vin0_scalarMemReqs_put_0_1_memReqAMOInfo_amoAcquire_107;
  wire [0:0] vin0_scalarMemReqs_put_0_1_memReqAMOInfo_amoRelease_107;
  wire [0:0] vin0_scalarMemReqs_put_0_1_memReqAMOInfo_amoNeedsResp_107;
  wire [31:0] vin0_scalarMemReqs_put_0_1_memReqAddr_107;
  wire [31:0] vin0_scalarMemReqs_put_0_1_memReqData_107;
  wire [0:0] vin0_scalarMemReqs_put_0_1_memReqDataTagBit_107;
  wire [0:0] vin0_scalarMemReqs_put_0_1_memReqDataTagBitMask_107;
  wire [0:0] vin0_scalarMemReqs_put_0_1_memReqIsUnsigned_107;
  wire [0:0] vin0_scalarMemReqs_put_0_1_memReqIsFinal_107;
  wire [0:0] vin0_scalarMemReqs_put_en_107;
  wire [0:0] vin0_scalarMemResps_consume_en_107;
  wire [0:0] vin0_scalarSIMTResps_consume_en_107;
  wire [0:0] vout_scalarUartOut_canPeek_107;
  wire [7:0] vout_scalarUartOut_peek_107;
  wire [0:0] vout_scalarSIMTReqs_canPeek_107;
  wire [1:0] vout_scalarSIMTReqs_peek_simtReqCmd_0_107;
  wire [31:0] vout_scalarSIMTReqs_peek_simtReqAddr_107;
  wire [31:0] vout_scalarSIMTReqs_peek_simtReqData_107;
  wire [0:0] v_108;
  wire [0:0] v_109;
  wire [0:0] v_112;
  wire [0:0] v_113;
  wire [0:0] v_116;
  wire [0:0] v_117;
  wire [0:0] v_118;
  wire [0:0] v_120;
  wire [0:0] v_121;
  wire [0:0] v_122;
  wire [0:0] v_124;
  wire [0:0] v_125;
  wire [0:0] v_126;
  wire [0:0] v_128;
  wire [31:0] v_130;
  wire [0:0] v_132;
  wire [0:0] v_133;
  wire [0:0] v_134;
  wire [2:0] v_135;
  wire [7:0] v_149 = 8'bxxxxxxxx;
  wire [1:0] v_150 = 2'bxx;
  wire [2:0] v_151 = 3'bxxx;
  wire [4:0] v_152 = 5'bxxxxx;
  wire [0:0] v_153 = 1'bx;
  wire [0:0] v_154 = 1'bx;
  wire [0:0] v_155 = 1'bx;
  wire [31:0] v_156 = 32'bxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx;
  wire [31:0] v_157 = 32'bxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx;
  wire [0:0] v_158 = 1'bx;
  wire [0:0] v_159 = 1'bx;
  wire [0:0] v_160 = 1'bx;
  wire [0:0] v_161 = 1'bx;
  // Instances
  //////////////////////////////////////////////////////////////////////////////
  assign v_0 = ~(1'h0);
  assign v_1 = (v_0 == 1 ? (1'h0) : 1'h0);
  assign v_2 = (1'h1) & v_1;
  assign v_3 = in0_cpuDomainUARTIns_avl_jtaguart_readdata;
  assign v_4 = v_3[15:15];
  assign v_5 = in0_cpuDomainUARTIns_avl_jtaguart_waitrequest;
  assign v_6 = ~v_5;
  assign v_7 = ~v_54;
  assign v_8 = v_7 | (1'h0);
  assign v_9 = v_35 == (2'h0);
  assign v_10 = v_9 & (1'h1);
  assign v_11 = ~v_13;
  assign v_12 = (v_10 == 1 ? v_11 : 1'h0);
  assign v_14 = ~v_13;
  assign v_15 = v_10 & v_14;
  assign v_16 = v_8 & v_15;
  assign v_17 = v_38 | v_16;
  assign v_18 = v_10 & v_13;
  assign v_19 = vout_scalarUartOut_canPeek_107 & v_18;
  assign v_20 = ~v_5;
  assign v_21 = v_35 == (2'h3);
  assign v_22 = v_21 & (1'h1);
  assign v_23 = v_20 & v_22;
  assign v_24 = ~v_5;
  assign v_25 = v_35 == (2'h2);
  assign v_26 = v_25 & (1'h1);
  assign v_27 = v_24 & v_26;
  assign v_28 = v_23 | v_27;
  assign v_29 = v_19 | v_28;
  assign v_30 = v_17 | v_29;
  assign v_31 = v_3[31:16];
  assign v_32 = (16'h0) < v_31;
  assign v_33 = v_32 ? (2'h3) : (2'h0);
  assign v_34 = (v_19 == 1 ? (2'h2) : 2'h0)
                |
                (v_16 == 1 ? (2'h1) : 2'h0)
                |
                (v_38 == 1 ? (2'h0) : 2'h0)
                |
                (v_27 == 1 ? v_33 : 2'h0)
                |
                (v_23 == 1 ? (2'h0) : 2'h0);
  assign v_36 = v_35 == (2'h1);
  assign v_37 = v_36 & (1'h1);
  assign v_38 = v_6 & v_37;
  assign act_39 = v_4 & v_38;
  assign v_40 = ~v_1;
  assign v_41 = (1'h1) & v_40;
  assign v_42 = act_39 & v_41;
  assign v_43 = ~act_39;
  assign v_44 = vin0_scalarUartIn_consume_en_107 & (1'h1);
  assign v_45 = ~v_44;
  assign v_46 = (v_44 == 1 ? (1'h1) : 1'h0)
                |
                (v_45 == 1 ? (1'h0) : 1'h0);
  assign v_47 = ~v_54;
  assign v_48 = v_46 | v_47;
  assign v_49 = v_43 & v_48;
  assign v_50 = v_49 & v_41;
  assign v_51 = v_42 | v_50;
  assign v_52 = v_2 | v_51;
  assign v_53 = (v_2 == 1 ? (1'h0) : 1'h0)
                |
                (v_50 == 1 ? (1'h0) : 1'h0)
                |
                (v_42 == 1 ? (1'h1) : 1'h0);
  assign v_55 = ~act_39;
  assign v_56 = v_3[7:0];
  assign v_57 = (act_39 == 1 ? v_56 : 8'h0)
                |
                (v_55 == 1 ? v_149 : 8'h0);
  assign v_58 = (v_42 == 1 ? v_57 : 8'h0);
  assign v_60 = in0_cpuDomainFromDRAM_canPeek;
  assign v_61 = in0_cpuDomainFromDRAM_peek_dramRespBurstId;
  assign v_62 = in0_cpuDomainFromDRAM_peek_dramRespData;
  assign v_63 = in0_cpuDomainFromDRAM_peek_dramRespDataTagBits;
  assign v_64 = vin0_scalarMemReqs_put_en_107 & (1'h1);
  assign v_65 = ~v_64;
  assign v_66 = (v_64 == 1 ? vin0_scalarMemReqs_put_0_1_memReqAccessWidth_107 : 2'h0)
                |
                (v_65 == 1 ? v_150 : 2'h0);
  assign v_67 = ~v_64;
  assign v_68 = (v_64 == 1 ? vin0_scalarMemReqs_put_0_1_memReqOp_107 : 3'h0)
                |
                (v_67 == 1 ? v_151 : 3'h0);
  assign v_69 = ~v_64;
  assign v_70 = (v_64 == 1 ? vin0_scalarMemReqs_put_0_1_memReqAMOInfo_amoOp_107 : 5'h0)
                |
                (v_69 == 1 ? v_152 : 5'h0);
  assign v_71 = ~v_64;
  assign v_72 = (v_64 == 1 ? vin0_scalarMemReqs_put_0_1_memReqAMOInfo_amoAcquire_107 : 1'h0)
                |
                (v_71 == 1 ? v_153 : 1'h0);
  assign v_73 = ~v_64;
  assign v_74 = (v_64 == 1 ? vin0_scalarMemReqs_put_0_1_memReqAMOInfo_amoRelease_107 : 1'h0)
                |
                (v_73 == 1 ? v_154 : 1'h0);
  assign v_75 = ~v_64;
  assign v_76 = (v_64 == 1 ? vin0_scalarMemReqs_put_0_1_memReqAMOInfo_amoNeedsResp_107 : 1'h0)
                |
                (v_75 == 1 ? v_155 : 1'h0);
  assign v_77 = ~v_64;
  assign v_78 = (v_64 == 1 ? vin0_scalarMemReqs_put_0_1_memReqAddr_107 : 32'h0)
                |
                (v_77 == 1 ? v_156 : 32'h0);
  assign v_79 = ~v_64;
  assign v_80 = (v_64 == 1 ? vin0_scalarMemReqs_put_0_1_memReqData_107 : 32'h0)
                |
                (v_79 == 1 ? v_157 : 32'h0);
  assign v_81 = ~v_64;
  assign v_82 = (v_64 == 1 ? vin0_scalarMemReqs_put_0_1_memReqDataTagBit_107 : 1'h0)
                |
                (v_81 == 1 ? v_158 : 1'h0);
  assign v_83 = ~v_64;
  assign v_84 = (v_64 == 1 ? vin0_scalarMemReqs_put_0_1_memReqDataTagBitMask_107 : 1'h0)
                |
                (v_83 == 1 ? v_159 : 1'h0);
  assign v_85 = ~v_64;
  assign v_86 = (v_64 == 1 ? vin0_scalarMemReqs_put_0_1_memReqIsUnsigned_107 : 1'h0)
                |
                (v_85 == 1 ? v_160 : 1'h0);
  assign v_87 = ~v_64;
  assign v_88 = (v_64 == 1 ? vin0_scalarMemReqs_put_0_1_memReqIsFinal_107 : 1'h0)
                |
                (v_87 == 1 ? v_161 : 1'h0);
  assign v_89 = ~v_64;
  assign v_90 = (v_64 == 1 ? (1'h1) : 1'h0)
                |
                (v_89 == 1 ? (1'h0) : 1'h0);
  assign v_91 = vin0_scalarMemResps_consume_en_107 & (1'h1);
  assign v_92 = ~v_91;
  assign v_93 = (v_91 == 1 ? (1'h1) : 1'h0)
                |
                (v_92 == 1 ? (1'h0) : 1'h0);
  assign v_94 = out_cpuDomainToDRAM_consume_en;
  assign v_95 = v_94 & (1'h1);
  assign v_96 = ~v_95;
  assign v_97 = (v_95 == 1 ? (1'h1) : 1'h0)
                |
                (v_96 == 1 ? (1'h0) : 1'h0);
  CPUDataCache
    CPUDataCache_98
      (.clock(clock),
       .reset(reset),
       .in0_canPeek(v_60),
       .in0_peek_dramRespBurstId(v_61),
       .in0_peek_dramRespData(v_62),
       .in0_peek_dramRespDataTagBits(v_63),
       .out_0_put_0_1_memReqAccessWidth(v_66),
       .out_0_put_0_1_memReqOp(v_68),
       .out_0_put_0_1_memReqAMOInfo_amoOp(v_70),
       .out_0_put_0_1_memReqAMOInfo_amoAcquire(v_72),
       .out_0_put_0_1_memReqAMOInfo_amoRelease(v_74),
       .out_0_put_0_1_memReqAMOInfo_amoNeedsResp(v_76),
       .out_0_put_0_1_memReqAddr(v_78),
       .out_0_put_0_1_memReqData(v_80),
       .out_0_put_0_1_memReqDataTagBit(v_82),
       .out_0_put_0_1_memReqDataTagBitMask(v_84),
       .out_0_put_0_1_memReqIsUnsigned(v_86),
       .out_0_put_0_1_memReqIsFinal(v_88),
       .out_0_put_en(v_90),
       .out_1_consume_en(v_93),
       .out_2_consume_en(v_97),
       .in0_consume_en(vin0_consume_en_98),
       .out_0_canPut(vout_0_canPut_98),
       .out_1_canPeek(vout_1_canPeek_98),
       .out_1_peek_1_memRespData(vout_1_peek_1_memRespData_98),
       .out_1_peek_1_memRespDataTagBit(vout_1_peek_1_memRespDataTagBit_98),
       .out_1_peek_1_memRespIsFinal(vout_1_peek_1_memRespIsFinal_98),
       .out_2_canPeek(vout_2_canPeek_98),
       .out_2_peek_dramReqIsStore(vout_2_peek_dramReqIsStore_98),
       .out_2_peek_dramReqAddr(vout_2_peek_dramReqAddr_98),
       .out_2_peek_dramReqData(vout_2_peek_dramReqData_98),
       .out_2_peek_dramReqDataTagBits(vout_2_peek_dramReqDataTagBits_98),
       .out_2_peek_dramReqByteEn(vout_2_peek_dramReqByteEn_98),
       .out_2_peek_dramReqBurst(vout_2_peek_dramReqBurst_98),
       .out_2_peek_dramReqIsFinal(vout_2_peek_dramReqIsFinal_98));
  assign v_99 = in0_cpuDomainFromSIMT_canPeek;
  assign v_100 = in0_cpuDomainFromSIMT_peek;
  assign v_101 = ~v_23;
  assign v_102 = (v_23 == 1 ? (1'h1) : 1'h0)
                 |
                 (v_101 == 1 ? (1'h0) : 1'h0);
  assign v_103 = out_cpuDomainToSIMT_consume_en;
  assign v_104 = v_103 & (1'h1);
  assign v_105 = ~v_104;
  assign v_106 = (v_104 == 1 ? (1'h1) : 1'h0)
                 |
                 (v_105 == 1 ? (1'h0) : 1'h0);
  CPUCore
    CPUCore_107
      (.clock(clock),
       .reset(reset),
       .in0_scalarUartIn_canPeek(v_54),
       .in0_scalarUartIn_peek(v_59),
       .in0_scalarMemReqs_canPut(vout_0_canPut_98),
       .in0_scalarMemResps_canPeek(vout_1_canPeek_98),
       .in0_scalarMemResps_peek_1_memRespData(vout_1_peek_1_memRespData_98),
       .in0_scalarMemResps_peek_1_memRespDataTagBit(vout_1_peek_1_memRespDataTagBit_98),
       .in0_scalarMemResps_peek_1_memRespIsFinal(vout_1_peek_1_memRespIsFinal_98),
       .in0_scalarSIMTResps_canPeek(v_99),
       .in0_scalarSIMTResps_peek(v_100),
       .out_scalarUartOut_consume_en(v_102),
       .out_scalarSIMTReqs_consume_en(v_106),
       .in0_scalarUartIn_consume_en(vin0_scalarUartIn_consume_en_107),
       .in0_scalarMemReqs_put_0_1_memReqAccessWidth(vin0_scalarMemReqs_put_0_1_memReqAccessWidth_107),
       .in0_scalarMemReqs_put_0_1_memReqOp(vin0_scalarMemReqs_put_0_1_memReqOp_107),
       .in0_scalarMemReqs_put_0_1_memReqAMOInfo_amoOp(vin0_scalarMemReqs_put_0_1_memReqAMOInfo_amoOp_107),
       .in0_scalarMemReqs_put_0_1_memReqAMOInfo_amoAcquire(vin0_scalarMemReqs_put_0_1_memReqAMOInfo_amoAcquire_107),
       .in0_scalarMemReqs_put_0_1_memReqAMOInfo_amoRelease(vin0_scalarMemReqs_put_0_1_memReqAMOInfo_amoRelease_107),
       .in0_scalarMemReqs_put_0_1_memReqAMOInfo_amoNeedsResp(vin0_scalarMemReqs_put_0_1_memReqAMOInfo_amoNeedsResp_107),
       .in0_scalarMemReqs_put_0_1_memReqAddr(vin0_scalarMemReqs_put_0_1_memReqAddr_107),
       .in0_scalarMemReqs_put_0_1_memReqData(vin0_scalarMemReqs_put_0_1_memReqData_107),
       .in0_scalarMemReqs_put_0_1_memReqDataTagBit(vin0_scalarMemReqs_put_0_1_memReqDataTagBit_107),
       .in0_scalarMemReqs_put_0_1_memReqDataTagBitMask(vin0_scalarMemReqs_put_0_1_memReqDataTagBitMask_107),
       .in0_scalarMemReqs_put_0_1_memReqIsUnsigned(vin0_scalarMemReqs_put_0_1_memReqIsUnsigned_107),
       .in0_scalarMemReqs_put_0_1_memReqIsFinal(vin0_scalarMemReqs_put_0_1_memReqIsFinal_107),
       .in0_scalarMemReqs_put_en(vin0_scalarMemReqs_put_en_107),
       .in0_scalarMemResps_consume_en(vin0_scalarMemResps_consume_en_107),
       .in0_scalarSIMTResps_consume_en(vin0_scalarSIMTResps_consume_en_107),
       .out_scalarUartOut_canPeek(vout_scalarUartOut_canPeek_107),
       .out_scalarUartOut_peek(vout_scalarUartOut_peek_107),
       .out_scalarSIMTReqs_canPeek(vout_scalarSIMTReqs_canPeek_107),
       .out_scalarSIMTReqs_peek_simtReqCmd_0(vout_scalarSIMTReqs_peek_simtReqCmd_0_107),
       .out_scalarSIMTReqs_peek_simtReqAddr(vout_scalarSIMTReqs_peek_simtReqAddr_107),
       .out_scalarSIMTReqs_peek_simtReqData(vout_scalarSIMTReqs_peek_simtReqData_107));
  assign v_108 = ~v_8;
  assign v_109 = v_108 & v_37;
  assign v_112 = ~vout_scalarUartOut_canPeek_107;
  assign v_113 = v_112 & v_22;
  assign v_116 = vin0_consume_en_98 & (1'h1);
  assign v_117 = ~v_116;
  assign v_118 = (v_116 == 1 ? (1'h1) : 1'h0)
                 |
                 (v_117 == 1 ? (1'h0) : 1'h0);
  assign in0_cpuDomainFromDRAM_consume_en = v_118;
  assign v_120 = vin0_scalarSIMTResps_consume_en_107 & (1'h1);
  assign v_121 = ~v_120;
  assign v_122 = (v_120 == 1 ? (1'h1) : 1'h0)
                 |
                 (v_121 == 1 ? (1'h0) : 1'h0);
  assign in0_cpuDomainFromSIMT_consume_en = v_122;
  assign v_124 = v_35 == (2'h1);
  assign v_125 = v_35 == (2'h2);
  assign v_126 = v_124 | v_125;
  assign out_cpuDomainUARTOuts_avl_jtaguart_read = v_126;
  assign v_128 = v_35 == (2'h3);
  assign out_cpuDomainUARTOuts_avl_jtaguart_write = v_128;
  assign v_130 = {{24{1'b0}}, vout_scalarUartOut_peek_107};
  assign out_cpuDomainUARTOuts_avl_jtaguart_writedata = v_130;
  assign v_132 = v_35 == (2'h1);
  assign v_133 = v_35 == (2'h3);
  assign v_134 = v_132 | v_133;
  assign v_135 = v_134 ? (3'h0) : (3'h4);
  assign out_cpuDomainUARTOuts_avl_jtaguart_address = v_135;
  assign out_cpuDomainToDRAM_canPeek = vout_2_canPeek_98;
  assign out_cpuDomainToDRAM_peek_dramReqIsStore = vout_2_peek_dramReqIsStore_98;
  assign out_cpuDomainToDRAM_peek_dramReqAddr = vout_2_peek_dramReqAddr_98;
  assign out_cpuDomainToDRAM_peek_dramReqData = vout_2_peek_dramReqData_98;
  assign out_cpuDomainToDRAM_peek_dramReqDataTagBits = vout_2_peek_dramReqDataTagBits_98;
  assign out_cpuDomainToDRAM_peek_dramReqByteEn = vout_2_peek_dramReqByteEn_98;
  assign out_cpuDomainToDRAM_peek_dramReqBurst = vout_2_peek_dramReqBurst_98;
  assign out_cpuDomainToDRAM_peek_dramReqIsFinal = vout_2_peek_dramReqIsFinal_98;
  assign out_cpuDomainToSIMT_canPeek = vout_scalarSIMTReqs_canPeek_107;
  assign out_cpuDomainToSIMT_peek_simtReqCmd_0 = vout_scalarSIMTReqs_peek_simtReqCmd_0_107;
  assign out_cpuDomainToSIMT_peek_simtReqAddr = vout_scalarSIMTReqs_peek_simtReqAddr_107;
  assign out_cpuDomainToSIMT_peek_simtReqData = vout_scalarSIMTReqs_peek_simtReqData_107;
  // Always block
  //////////////////////////////////////////////////////////////////////////////
  always @(posedge clock) begin
    if (reset) begin v_13 <= 1'h0; v_35 <= 2'h0; v_54 <= 1'h0;
    end else begin
      if (v_10 == 1) v_13 <= v_12;
      if (v_30 == 1) v_35 <= v_34;
      if (v_52 == 1) v_54 <= v_53;
      if (v_42 == 1) v_59 <= v_58;
      if (v_109 == 1) begin
        $write ("Assertion failed: JTAGUART: internal error 1\n");
      end
      if (v_109 == 1) $finish;
      if (v_113 == 1) begin
        $write ("Assertion failed: JTAGUART: internal error 2\n");
      end
      if (v_113 == 1) $finish;
    end
  end
endmodule