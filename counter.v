// 64-bit counter
module counter(clk, count);
	input clk;
	output [63:0] count;
	reg [63:0] count;

	initial //initialize count to 0
		count = 0;

	always @(posedge clk) //increment counter at each clock edge
	begin
		count <= count + 1;
	end
endmodule
