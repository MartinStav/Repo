local my_proto = Proto("stavrovsky", "My Protocol")

local f_msg_type = ProtoField.uint8("stavrovsky.msg_type", "Message Type", base.DEC)
local f_flag = ProtoField.uint8("stavrovsky.flag", "Flag", base.DEC)
local f_seq_num = ProtoField.uint16("stavrovsky.seq_num", "Sequence Number", base.DEC)
local f_crc = ProtoField.uint16("stavrovsky.crc", "CRC", base.HEX)
local f_frag_num = ProtoField.uint32("stavrovsky.frag_num", "Fragment Number", base.DEC)

my_proto.fields = {f_msg_type, f_flag, f_seq_num, f_crc, f_frag_num}

function my_proto.dissector(buffer, pinfo, tree)
    if buffer:len() < 8 then return end

    pinfo.cols.protocol = "Stavrovsky Protocol"

    local subtree = tree:add(my_proto, buffer(), "My Custom Protocol")

    local header = buffer(0, 2):uint()

    local msg_type = bit.rshift(bit.band(header, 0xE000), 13)
    local flag = bit.rshift(bit.band(header, 0x1000), 12)
    local seq_num = bit.band(header, 0x0FFF)

    subtree:add(f_msg_type, buffer(0, 2), msg_type)
    subtree:add(f_flag, buffer(0, 2), flag)
    subtree:add(f_seq_num, buffer(0, 2), seq_num)
    subtree:add(f_crc, buffer(2, 2))
    subtree:add(f_frag_num, buffer(4, 4))

    if msg_type == 1 then
        pinfo.cols.info:append(" SYN")
    elseif msg_type == 2 then
        pinfo.cols.info:append(" SYN-ACK")
    elseif msg_type == 3 then
        pinfo.cols.info:append(" ACK")
    elseif msg_type == 4 then
        pinfo.cols.info:append(" NACK")
    elseif msg_type == 5 then
        pinfo.cols.info:append(" Message")
    elseif msg_type == 6 then
        pinfo.cols.info:append(" File")
    elseif msg_type == 7 then
        pinfo.cols.info:append(" Heartbeat")
    elseif msg_type == 0 then
        pinfo.cols.info:append(" Close Connection")
    end
end

local udp_port = DissectorTable.get("udp.port")
udp_port:add(5001, my_proto)
