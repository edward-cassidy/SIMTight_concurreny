module SIMTight
  (input wire clock,
   input wire reset,
   input wire [0:0] in0_socCPUClk,
   input wire [0:0] in0_socCPURst,
   input wire [31:0] in0_socUARTIns_avl_jtaguart_readdata,
   input wire [0:0] in0_socUARTIns_avl_jtaguart_waitrequest,
   input wire [0:0] in0_socSIMTClk,
   input wire [0:0] in0_socSIMTRst,
   input wire [511:0] in0_socDRAMIns_avl_dram_readdata,
   input wire [0:0] in0_socDRAMIns_avl_dram_readdatavalid,
   input wire [0:0] in0_socDRAMIns_avl_dram_waitrequest,
   output wire [0:0] out_socUARTOuts_avl_jtaguart_read,
   output wire [0:0] out_socUARTOuts_avl_jtaguart_write,
   output wire [31:0] out_socUARTOuts_avl_jtaguart_writedata,
   output wire [2:0] out_socUARTOuts_avl_jtaguart_address,
   output wire [0:0] out_socDRAMOuts_avl_dram_read,
   output wire [0:0] out_socDRAMOuts_avl_dram_write,
   output wire [511:0] out_socDRAMOuts_avl_dram_writedata,
   output wire [25:0] out_socDRAMOuts_avl_dram_address,
   output wire [63:0] out_socDRAMOuts_avl_dram_byteen,
   output wire [3:0] out_socDRAMOuts_avl_dram_burstcount);
  // Declarations
  //////////////////////////////////////////////////////////////////////////////
  wire [0:0] v_0;
  wire [0:0] v_1;
  wire [31:0] v_2;
  wire [0:0] v_3;
  wire [0:0] v_4;
  wire [0:0] v_5;
  wire [511:0] v_6;
  wire [0:0] v_7;
  wire [0:0] v_8;
  wire [63:0] v_9;
  wire [65:0] v_10;
  wire [0:0] v_11;
  wire [0:0] v_12;
  wire [0:0] v_13;
  wire [0:0] vin_ready_14;
  wire [0:0] vout_valid_14;
  wire [65:0] vout_data_14;
  wire [1:0] v_15;
  wire [63:0] v_16;
  wire [31:0] v_17;
  wire [31:0] v_18;
  wire [0:0] v_19;
  wire [537:0] v_20;
  wire [538:0] v_21;
  wire [79:0] v_22;
  wire [4:0] v_23;
  wire [84:0] v_24;
  wire [623:0] v_25;
  wire [0:0] v_26;
  wire [0:0] v_27;
  wire [0:0] v_28;
  wire [0:0] vin_ready_29;
  wire [0:0] vout_valid_29;
  wire [623:0] vout_data_29;
  wire [538:0] v_30;
  wire [0:0] v_31;
  wire [0:0] v_32;
  wire [537:0] v_33;
  wire [25:0] v_34;
  wire [511:0] v_35;
  wire [84:0] v_36;
  wire [79:0] v_37;
  wire [15:0] v_38;
  wire [63:0] v_39;
  wire [4:0] v_40;
  wire [3:0] v_41;
  wire [0:0] v_42;
  wire [0:0] v_43;
  wire [0:0] v_44;
  wire [0:0] v_45;
  wire [0:0] vin_ready_46;
  wire [0:0] vout_valid_46;
  wire [31:0] vout_data_46;
  wire [0:0] v_47;
  wire [0:0] v_48;
  wire [0:0] v_49;
  wire [0:0] v_50;
  wire [0:0] v_51;
  wire [0:0] v_52;
  wire [0:0] v_53;
  wire [0:0] v_54;
  wire [0:0] vin0_simtDomainMgmtReqsFromCPU_consume_en_55;
  wire [0:0] vin0_simtDomainDRAMReqsFromCPU_consume_en_55;
  wire [0:0] vout_simtDomainDRAMOuts_avl_dram_read_55;
  wire [0:0] vout_simtDomainDRAMOuts_avl_dram_write_55;
  wire [511:0] vout_simtDomainDRAMOuts_avl_dram_writedata_55;
  wire [25:0] vout_simtDomainDRAMOuts_avl_dram_address_55;
  wire [63:0] vout_simtDomainDRAMOuts_avl_dram_byteen_55;
  wire [3:0] vout_simtDomainDRAMOuts_avl_dram_burstcount_55;
  wire [0:0] vout_simtDomainMgmtRespsToCPU_canPeek_55;
  wire [31:0] vout_simtDomainMgmtRespsToCPU_peek_55;
  wire [0:0] vout_simtDomainDRAMRespsToCPU_canPeek_55;
  wire [3:0] vout_simtDomainDRAMRespsToCPU_peek_dramRespBurstId_55;
  wire [511:0] vout_simtDomainDRAMRespsToCPU_peek_dramRespData_55;
  wire [15:0] vout_simtDomainDRAMRespsToCPU_peek_dramRespDataTagBits_55;
  wire [3:0] v_56;
  wire [527:0] v_57;
  wire [531:0] v_58;
  wire [0:0] v_59;
  wire [0:0] v_60;
  wire [0:0] v_61;
  wire [0:0] vin_ready_62;
  wire [0:0] vout_valid_62;
  wire [531:0] vout_data_62;
  wire [3:0] v_63;
  wire [3:0] v_64;
  wire [527:0] v_65;
  wire [511:0] v_66;
  wire [15:0] v_67;
  wire [0:0] v_68;
  wire [0:0] v_69;
  wire [0:0] v_70;
  wire [0:0] v_71;
  wire [0:0] v_72;
  wire [0:0] v_73;
  wire [0:0] v_74;
  wire [0:0] v_75;
  wire [0:0] vin0_cpuDomainFromDRAM_consume_en_76;
  wire [0:0] vin0_cpuDomainFromSIMT_consume_en_76;
  wire [0:0] vout_cpuDomainUARTOuts_avl_jtaguart_read_76;
  wire [0:0] vout_cpuDomainUARTOuts_avl_jtaguart_write_76;
  wire [31:0] vout_cpuDomainUARTOuts_avl_jtaguart_writedata_76;
  wire [2:0] vout_cpuDomainUARTOuts_avl_jtaguart_address_76;
  wire [0:0] vout_cpuDomainToDRAM_canPeek_76;
  wire [0:0] vout_cpuDomainToDRAM_peek_dramReqIsStore_76;
  wire [25:0] vout_cpuDomainToDRAM_peek_dramReqAddr_76;
  wire [511:0] vout_cpuDomainToDRAM_peek_dramReqData_76;
  wire [15:0] vout_cpuDomainToDRAM_peek_dramReqDataTagBits_76;
  wire [63:0] vout_cpuDomainToDRAM_peek_dramReqByteEn_76;
  wire [3:0] vout_cpuDomainToDRAM_peek_dramReqBurst_76;
  wire [0:0] vout_cpuDomainToDRAM_peek_dramReqIsFinal_76;
  wire [0:0] vout_cpuDomainToSIMT_canPeek_76;
  wire [1:0] vout_cpuDomainToSIMT_peek_simtReqCmd_0_76;
  wire [31:0] vout_cpuDomainToSIMT_peek_simtReqAddr_76;
  wire [31:0] vout_cpuDomainToSIMT_peek_simtReqData_76;
  // Instances
  //////////////////////////////////////////////////////////////////////////////
  assign v_0 = in0_socCPUClk;
  assign v_1 = in0_socCPURst;
  assign v_2 = in0_socUARTIns_avl_jtaguart_readdata;
  assign v_3 = in0_socUARTIns_avl_jtaguart_waitrequest;
  assign v_4 = in0_socSIMTClk;
  assign v_5 = in0_socSIMTRst;
  assign v_6 = in0_socDRAMIns_avl_dram_readdata;
  assign v_7 = in0_socDRAMIns_avl_dram_readdatavalid;
  assign v_8 = in0_socDRAMIns_avl_dram_waitrequest;
  assign v_9 = {vout_cpuDomainToSIMT_peek_simtReqAddr_76, vout_cpuDomainToSIMT_peek_simtReqData_76};
  assign v_10 = {vout_cpuDomainToSIMT_peek_simtReqCmd_0_76, v_9};
  assign v_11 = vin0_simtDomainMgmtReqsFromCPU_consume_en_55 & (1'h1);
  assign v_12 = ~v_11;
  assign v_13 = (v_11 == 1 ? (1'h1) : 1'h0)
                |
                (v_12 == 1 ? (1'h0) : 1'h0);
  AvalonStreamClockCrosser# (.DATA_WIDTH(66))
    AvalonStreamClockCrosser_14
      (.in_clk(v_0),
       .in_reset(v_1),
       .out_clk(v_4),
       .out_reset(v_5),
       .in_valid(vout_cpuDomainToSIMT_canPeek_76),
       .in_data(v_10),
       .out_ready(v_13),
       .in_ready(vin_ready_14),
       .out_valid(vout_valid_14),
       .out_data(vout_data_14));
  assign v_15 = vout_data_14[65:64];
  assign v_16 = vout_data_14[63:0];
  assign v_17 = v_16[63:32];
  assign v_18 = v_16[31:0];
  assign v_19 = vout_cpuDomainToDRAM_peek_dramReqIsStore_76;
  assign v_20 = {vout_cpuDomainToDRAM_peek_dramReqAddr_76, vout_cpuDomainToDRAM_peek_dramReqData_76};
  assign v_21 = {v_19, v_20};
  assign v_22 = {vout_cpuDomainToDRAM_peek_dramReqDataTagBits_76, vout_cpuDomainToDRAM_peek_dramReqByteEn_76};
  assign v_23 = {vout_cpuDomainToDRAM_peek_dramReqBurst_76, vout_cpuDomainToDRAM_peek_dramReqIsFinal_76};
  assign v_24 = {v_22, v_23};
  assign v_25 = {v_21, v_24};
  assign v_26 = vin0_simtDomainDRAMReqsFromCPU_consume_en_55 & (1'h1);
  assign v_27 = ~v_26;
  assign v_28 = (v_26 == 1 ? (1'h1) : 1'h0)
                |
                (v_27 == 1 ? (1'h0) : 1'h0);
  AvalonStreamClockCrosser# (.DATA_WIDTH(624))
    AvalonStreamClockCrosser_29
      (.in_clk(v_0),
       .in_reset(v_1),
       .out_clk(v_4),
       .out_reset(v_5),
       .in_valid(vout_cpuDomainToDRAM_canPeek_76),
       .in_data(v_25),
       .out_ready(v_28),
       .in_ready(vin_ready_29),
       .out_valid(vout_valid_29),
       .out_data(vout_data_29));
  assign v_30 = vout_data_29[623:85];
  assign v_31 = v_30[538:538];
  assign v_32 = v_31[0:0];
  assign v_33 = v_30[537:0];
  assign v_34 = v_33[537:512];
  assign v_35 = v_33[511:0];
  assign v_36 = vout_data_29[84:0];
  assign v_37 = v_36[84:5];
  assign v_38 = v_37[79:64];
  assign v_39 = v_37[63:0];
  assign v_40 = v_36[4:0];
  assign v_41 = v_40[4:1];
  assign v_42 = v_40[0:0];
  assign v_43 = vin0_cpuDomainFromSIMT_consume_en_76 & (1'h1);
  assign v_44 = ~v_43;
  assign v_45 = (v_43 == 1 ? (1'h1) : 1'h0)
                |
                (v_44 == 1 ? (1'h0) : 1'h0);
  AvalonStreamClockCrosser# (.DATA_WIDTH(32))
    AvalonStreamClockCrosser_46
      (.in_clk(v_4),
       .in_reset(v_5),
       .out_clk(v_0),
       .out_reset(v_1),
       .in_valid(vout_simtDomainMgmtRespsToCPU_canPeek_55),
       .in_data(vout_simtDomainMgmtRespsToCPU_peek_55),
       .out_ready(v_45),
       .in_ready(vin_ready_46),
       .out_valid(vout_valid_46),
       .out_data(vout_data_46));
  assign v_47 = vout_simtDomainMgmtRespsToCPU_canPeek_55 & vin_ready_46;
  assign v_48 = v_47 & (1'h1);
  assign v_49 = ~v_48;
  assign v_50 = (v_48 == 1 ? (1'h1) : 1'h0)
                |
                (v_49 == 1 ? (1'h0) : 1'h0);
  assign v_51 = vout_simtDomainDRAMRespsToCPU_canPeek_55 & vin_ready_62;
  assign v_52 = v_51 & (1'h1);
  assign v_53 = ~v_52;
  assign v_54 = (v_52 == 1 ? (1'h1) : 1'h0)
                |
                (v_53 == 1 ? (1'h0) : 1'h0);
  SIMTDomain
    SIMTDomain_55
      (.clock(v_4),
       .reset(v_5),
       .in0_simtDomainDRAMIns_avl_dram_readdata(v_6),
       .in0_simtDomainDRAMIns_avl_dram_readdatavalid(v_7),
       .in0_simtDomainDRAMIns_avl_dram_waitrequest(v_8),
       .in0_simtDomainMgmtReqsFromCPU_canPeek(vout_valid_14),
       .in0_simtDomainMgmtReqsFromCPU_peek_simtReqCmd_0(v_15),
       .in0_simtDomainMgmtReqsFromCPU_peek_simtReqAddr(v_17),
       .in0_simtDomainMgmtReqsFromCPU_peek_simtReqData(v_18),
       .in0_simtDomainDRAMReqsFromCPU_canPeek(vout_valid_29),
       .in0_simtDomainDRAMReqsFromCPU_peek_dramReqIsStore(v_32),
       .in0_simtDomainDRAMReqsFromCPU_peek_dramReqAddr(v_34),
       .in0_simtDomainDRAMReqsFromCPU_peek_dramReqData(v_35),
       .in0_simtDomainDRAMReqsFromCPU_peek_dramReqDataTagBits(v_38),
       .in0_simtDomainDRAMReqsFromCPU_peek_dramReqByteEn(v_39),
       .in0_simtDomainDRAMReqsFromCPU_peek_dramReqBurst(v_41),
       .in0_simtDomainDRAMReqsFromCPU_peek_dramReqIsFinal(v_42),
       .out_simtDomainMgmtRespsToCPU_consume_en(v_50),
       .out_simtDomainDRAMRespsToCPU_consume_en(v_54),
       .in0_simtDomainMgmtReqsFromCPU_consume_en(vin0_simtDomainMgmtReqsFromCPU_consume_en_55),
       .in0_simtDomainDRAMReqsFromCPU_consume_en(vin0_simtDomainDRAMReqsFromCPU_consume_en_55),
       .out_simtDomainDRAMOuts_avl_dram_read(vout_simtDomainDRAMOuts_avl_dram_read_55),
       .out_simtDomainDRAMOuts_avl_dram_write(vout_simtDomainDRAMOuts_avl_dram_write_55),
       .out_simtDomainDRAMOuts_avl_dram_writedata(vout_simtDomainDRAMOuts_avl_dram_writedata_55),
       .out_simtDomainDRAMOuts_avl_dram_address(vout_simtDomainDRAMOuts_avl_dram_address_55),
       .out_simtDomainDRAMOuts_avl_dram_byteen(vout_simtDomainDRAMOuts_avl_dram_byteen_55),
       .out_simtDomainDRAMOuts_avl_dram_burstcount(vout_simtDomainDRAMOuts_avl_dram_burstcount_55),
       .out_simtDomainMgmtRespsToCPU_canPeek(vout_simtDomainMgmtRespsToCPU_canPeek_55),
       .out_simtDomainMgmtRespsToCPU_peek(vout_simtDomainMgmtRespsToCPU_peek_55),
       .out_simtDomainDRAMRespsToCPU_canPeek(vout_simtDomainDRAMRespsToCPU_canPeek_55),
       .out_simtDomainDRAMRespsToCPU_peek_dramRespBurstId(vout_simtDomainDRAMRespsToCPU_peek_dramRespBurstId_55),
       .out_simtDomainDRAMRespsToCPU_peek_dramRespData(vout_simtDomainDRAMRespsToCPU_peek_dramRespData_55),
       .out_simtDomainDRAMRespsToCPU_peek_dramRespDataTagBits(vout_simtDomainDRAMRespsToCPU_peek_dramRespDataTagBits_55));
  assign v_56 = vout_simtDomainDRAMRespsToCPU_peek_dramRespBurstId_55;
  assign v_57 = {vout_simtDomainDRAMRespsToCPU_peek_dramRespData_55, vout_simtDomainDRAMRespsToCPU_peek_dramRespDataTagBits_55};
  assign v_58 = {v_56, v_57};
  assign v_59 = vin0_cpuDomainFromDRAM_consume_en_76 & (1'h1);
  assign v_60 = ~v_59;
  assign v_61 = (v_59 == 1 ? (1'h1) : 1'h0)
                |
                (v_60 == 1 ? (1'h0) : 1'h0);
  AvalonStreamClockCrosser# (.DATA_WIDTH(532))
    AvalonStreamClockCrosser_62
      (.in_clk(v_4),
       .in_reset(v_5),
       .out_clk(v_0),
       .out_reset(v_1),
       .in_valid(vout_simtDomainDRAMRespsToCPU_canPeek_55),
       .in_data(v_58),
       .out_ready(v_61),
       .in_ready(vin_ready_62),
       .out_valid(vout_valid_62),
       .out_data(vout_data_62));
  assign v_63 = vout_data_62[531:528];
  assign v_64 = v_63[3:0];
  assign v_65 = vout_data_62[527:0];
  assign v_66 = v_65[527:16];
  assign v_67 = v_65[15:0];
  assign v_68 = vout_cpuDomainToDRAM_canPeek_76 & vin_ready_29;
  assign v_69 = v_68 & (1'h1);
  assign v_70 = ~v_69;
  assign v_71 = (v_69 == 1 ? (1'h1) : 1'h0)
                |
                (v_70 == 1 ? (1'h0) : 1'h0);
  assign v_72 = vout_cpuDomainToSIMT_canPeek_76 & vin_ready_14;
  assign v_73 = v_72 & (1'h1);
  assign v_74 = ~v_73;
  assign v_75 = (v_73 == 1 ? (1'h1) : 1'h0)
                |
                (v_74 == 1 ? (1'h0) : 1'h0);
  CPUDomain
    CPUDomain_76
      (.clock(v_0),
       .reset(v_1),
       .in0_cpuDomainUARTIns_avl_jtaguart_readdata(v_2),
       .in0_cpuDomainUARTIns_avl_jtaguart_waitrequest(v_3),
       .in0_cpuDomainFromDRAM_canPeek(vout_valid_62),
       .in0_cpuDomainFromDRAM_peek_dramRespBurstId(v_64),
       .in0_cpuDomainFromDRAM_peek_dramRespData(v_66),
       .in0_cpuDomainFromDRAM_peek_dramRespDataTagBits(v_67),
       .in0_cpuDomainFromSIMT_canPeek(vout_valid_46),
       .in0_cpuDomainFromSIMT_peek(vout_data_46),
       .out_cpuDomainToDRAM_consume_en(v_71),
       .out_cpuDomainToSIMT_consume_en(v_75),
       .in0_cpuDomainFromDRAM_consume_en(vin0_cpuDomainFromDRAM_consume_en_76),
       .in0_cpuDomainFromSIMT_consume_en(vin0_cpuDomainFromSIMT_consume_en_76),
       .out_cpuDomainUARTOuts_avl_jtaguart_read(vout_cpuDomainUARTOuts_avl_jtaguart_read_76),
       .out_cpuDomainUARTOuts_avl_jtaguart_write(vout_cpuDomainUARTOuts_avl_jtaguart_write_76),
       .out_cpuDomainUARTOuts_avl_jtaguart_writedata(vout_cpuDomainUARTOuts_avl_jtaguart_writedata_76),
       .out_cpuDomainUARTOuts_avl_jtaguart_address(vout_cpuDomainUARTOuts_avl_jtaguart_address_76),
       .out_cpuDomainToDRAM_canPeek(vout_cpuDomainToDRAM_canPeek_76),
       .out_cpuDomainToDRAM_peek_dramReqIsStore(vout_cpuDomainToDRAM_peek_dramReqIsStore_76),
       .out_cpuDomainToDRAM_peek_dramReqAddr(vout_cpuDomainToDRAM_peek_dramReqAddr_76),
       .out_cpuDomainToDRAM_peek_dramReqData(vout_cpuDomainToDRAM_peek_dramReqData_76),
       .out_cpuDomainToDRAM_peek_dramReqDataTagBits(vout_cpuDomainToDRAM_peek_dramReqDataTagBits_76),
       .out_cpuDomainToDRAM_peek_dramReqByteEn(vout_cpuDomainToDRAM_peek_dramReqByteEn_76),
       .out_cpuDomainToDRAM_peek_dramReqBurst(vout_cpuDomainToDRAM_peek_dramReqBurst_76),
       .out_cpuDomainToDRAM_peek_dramReqIsFinal(vout_cpuDomainToDRAM_peek_dramReqIsFinal_76),
       .out_cpuDomainToSIMT_canPeek(vout_cpuDomainToSIMT_canPeek_76),
       .out_cpuDomainToSIMT_peek_simtReqCmd_0(vout_cpuDomainToSIMT_peek_simtReqCmd_0_76),
       .out_cpuDomainToSIMT_peek_simtReqAddr(vout_cpuDomainToSIMT_peek_simtReqAddr_76),
       .out_cpuDomainToSIMT_peek_simtReqData(vout_cpuDomainToSIMT_peek_simtReqData_76));
  assign out_socUARTOuts_avl_jtaguart_read = vout_cpuDomainUARTOuts_avl_jtaguart_read_76;
  assign out_socUARTOuts_avl_jtaguart_write = vout_cpuDomainUARTOuts_avl_jtaguart_write_76;
  assign out_socUARTOuts_avl_jtaguart_writedata = vout_cpuDomainUARTOuts_avl_jtaguart_writedata_76;
  assign out_socUARTOuts_avl_jtaguart_address = vout_cpuDomainUARTOuts_avl_jtaguart_address_76;
  assign out_socDRAMOuts_avl_dram_read = vout_simtDomainDRAMOuts_avl_dram_read_55;
  assign out_socDRAMOuts_avl_dram_write = vout_simtDomainDRAMOuts_avl_dram_write_55;
  assign out_socDRAMOuts_avl_dram_writedata = vout_simtDomainDRAMOuts_avl_dram_writedata_55;
  assign out_socDRAMOuts_avl_dram_address = vout_simtDomainDRAMOuts_avl_dram_address_55;
  assign out_socDRAMOuts_avl_dram_byteen = vout_simtDomainDRAMOuts_avl_dram_byteen_55;
  assign out_socDRAMOuts_avl_dram_burstcount = vout_simtDomainDRAMOuts_avl_dram_burstcount_55;
  // Always block
  //////////////////////////////////////////////////////////////////////////////
  always @(posedge clock) begin
    if (reset) begin
    end else begin
    end
  end
endmodule