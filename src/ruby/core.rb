def Z(i)
    "%010d"%i
end


# 
# input == value
#
class Input_Equals_Number
    attr_accessor :input 
    attr_accessor :value
    attr_accessor :n

    def initialize(_input,_value,_n)
        @input  = _input
        @value  = _value
        @n      = _n
    end 

    def expand()
        (0...@n).collect{|i|
            if ((@value >> i) & 1 == 1) then
                "<#{@input}_#{Z(i)}> 0 "
            else
                "-<#{@input}_#{Z(i)}> 0 "
            end
        }
    end

end


#
# input != value
#
class Input_Not_Equals_Number
    attr_accessor :input 
    attr_accessor :value
    attr_accessor :n

    def initialize(_input,_value,_n)
        @input  = _input
        @value  = _value
        @n      = _n
    end 

    def expand()
        (0...@n).collect{|i|
            if ((@value >> i) & 1 == 1) then
                "-<#{@input}_#{Z(i)}> "
            else
                "<#{@input}_#{Z(i)}> "
            end
        }.join("")+" 0 "
    end

end


#
# carry_out == ( pop_cnt(in_a,in_b,carry_in) >= 2 )
#
class CarryOut_Equal_POPCNT_GREATER_THAN_2
    attr_accessor :in_a
    attr_accessor :in_b
    attr_accessor :carry_in
    attr_accessor :carry_out
    
    def initialize(_in_a,_in_b,_carry_in,_carry_out)
        @in_a      = _in_a
        @in_b      = _in_b
        @carry_in  = _carry_in
        @carry_out = _carry_out
    end

    def expand()
        [
            "-<#{@in_a}> -<#{@in_b}> -<#{@carry_in}>  <#{@carry_out}> 0 ",            
            "-<#{@in_a}> -<#{@in_b}>  <#{@carry_in}>  <#{@carry_out}> 0 ",            
            "-<#{@in_a}>  <#{@in_b}> -<#{@carry_in}>  <#{@carry_out}> 0 ",            
            "-<#{@in_a}>  <#{@in_b}>  <#{@carry_in}> -<#{@carry_out}> 0 ",            
            " <#{@in_a}> -<#{@in_b}> -<#{@carry_in}>  <#{@carry_out}> 0 ",            
            " <#{@in_a}> -<#{@in_b}>  <#{@carry_in}> -<#{@carry_out}> 0 ",            
            " <#{@in_a}>  <#{@in_b}> -<#{@carry_in}> -<#{@carry_out}> 0 ",            
            " <#{@in_a}>  <#{@in_b}>  <#{@carry_in}> -<#{@carry_out}> 0 ",            
        ]
    end
end

#
# result == in_a ^ in_b ^ carry_in
#
class Result_Equal_A_XOR_B_XOR_CarryIn
    attr_accessor :in_a
    attr_accessor :in_b
    attr_accessor :carry_in
    attr_accessor :result
    
    def initialize(_in_a,_in_b,_carry_in,_result)
        @in_a     = _in_a
        @in_b     = _in_b
        @carry_in = _carry_in
        @result   = _result
    end

    def expand()
        [
            "-<#{@in_a}> -<#{@in_b}> -<#{@carry_in}>  <#{@result}> 0 ",            
            "-<#{@in_a}> -<#{@in_b}>  <#{@carry_in}> -<#{@result}> 0 ",            
            "-<#{@in_a}>  <#{@in_b}> -<#{@carry_in}> -<#{@result}> 0 ",            
            "-<#{@in_a}>  <#{@in_b}>  <#{@carry_in}>  <#{@result}> 0 ",            
            " <#{@in_a}> -<#{@in_b}> -<#{@carry_in}> -<#{@result}> 0 ",            
            " <#{@in_a}> -<#{@in_b}>  <#{@carry_in}>  <#{@result}> 0 ",            
            " <#{@in_a}>  <#{@in_b}> -<#{@carry_in}>  <#{@result}> 0 ",            
            " <#{@in_a}>  <#{@in_b}>  <#{@carry_in}> -<#{@result}> 0 ",            
        ]
    end
end

#
#  in_a + in_b + carry_in == (result,carry_out)
#
class Add_1Bit
    attr_accessor :in_a
    attr_accessor :in_b
    attr_accessor :carry_in
    attr_accessor :result
    attr_accessor :carry_out

    def initialize(_in_a,_in_b,_carry_in,_result,_carry_out)
        @in_a      = _in_a
        @in_b      = _in_b
        @carry_in  = _carry_in
        @result    = _result
        @carry_out = _carry_out
    end

    def expand()
        [
            CarryOut_Equal_POPCNT_GREATER_THAN_2.new(@in_a,@in_b,@carry_in,@carry_out),
            Result_Equal_A_XOR_B_XOR_CarryIn.new(@in_a,@in_b,@carry_in,@result)
        ]
    end
end

#
# in_a + in_b == result
#
class Add_NBit
    attr_accessor :in_a
    attr_accessor :in_b
    attr_accessor :result
    attr_accessor :over_flow
    attr_accessor :n

    @@call_count = 0

    def initialize(_in_a,_in_b,_result,_over_flow,_n)
        @in_a        = _in_a
        @in_b        = _in_b
        @result      = _result
        @over_flow   = _over_flow
        @n           = _n
    end

    def expand()
        @@call_count += 1
        [
            "-<AddNBit_#{Z(@@call_count)}_carry_out_#{Z(0)}> 0 ",
            (0...@n).collect{|i|
                Add_1Bit.new(
                     "#{@in_a}_#{Z(i)}",
                     "#{@in_b}_#{Z(i)}",
                     "AddNBit_#{Z(@@call_count)}_carry_out_#{Z(i)}",
                     "#{result}_#{Z(i)}",
                     "AddNBit_#{Z(@@call_count)}_carry_out_#{Z(i+1)}"
                )
            },
            "-<#{@over_flow}>  <AddNBit_#{Z(@@call_count)}_carry_out_#{Z(@n)}> 0 ",
            " <#{@over_flow}> -<AddNBit_#{Z(@@call_count)}_carry_out_#{Z(@n)}> 0 "
        ].flatten
    end
end

#
#  ( in_a * in_b ) << shift == result
#
class Mul_NBit_1Bit_Shift
    attr_accessor :in_a
    attr_accessor :in_b
    attr_accessor :result
    attr_accessor :shift
    attr_accessor :n

    @@call_count = 0

    def initialize(_in_a,_in_b,_result,_shift,_n)
        @in_a        = _in_a
        @in_b        = _in_b
        @result      = _result
        @shift       = _shift
        @n           = _n
    end

    def expand()
        [
            (0...@shift).collect{|i|
                "-<#{result}_#{Z(i)}> 0 "
            },
            (0...@n).collect{|i|
                [
                    " <#{result}_#{Z(i+@shift)}> -<#{@in_a}_#{Z(i)}> -<#{in_b}> 0 ",
                    "-<#{result}_#{Z(i+@shift)}> -<#{@in_a}_#{Z(i)}>  <#{in_b}> 0 ",
                    "-<#{result}_#{Z(i+@shift)}>  <#{@in_a}_#{Z(i)}> -<#{in_b}> 0 ",
                    "-<#{result}_#{Z(i+@shift)}>  <#{@in_a}_#{Z(i)}>  <#{in_b}> 0 ",
                ]
            },
            ((@shift+@n)...(@n*2)).collect{|i|
                "-<#{result}_#{Z(i)}> 0 "
            }
        ].flatten
    
    end
end

#
# in_a * in_b == result
#
class Mul_NBit
    attr_accessor :in_a
    attr_accessor :in_b
    attr_accessor :result
    attr_accessor :over_flow
    attr_accessor :n

    @@call_count = 0

    def initialize(_in_a,_in_b,_result,_over_flow,_n)
        @in_a        = _in_a
        @in_b        = _in_b
        @result      = _result
        @over_flow   = _over_flow
        @n           = _n
    end

    def expand()
        @@call_count+=1
        [
            (0...@n).collect{|i|
                Mul_NBit_1Bit_Shift.new(@in_a,"#{@in_b}_#{Z(i)}","Mul_NBit_Accum1_#{Z(@@call_count)}_#{Z(i)}",i,@n)
            },
            (0...(@n*2)).collect{|i|
                "-<Mul_NBit_Accum2_#{Z(@@call_count)}_#{Z(0)}_#{Z(i)}> 0 "
            },
            (0...(@n)).collect{|i|
                Add_NBit.new("Mul_NBit_Accum1_#{Z(@@call_count)}_#{Z(i)}",
                             "Mul_NBit_Accum2_#{Z(@@call_count)}_#{Z(i)}",
                             "Mul_NBit_Accum2_#{Z(@@call_count)}_#{Z(i+1)}",
                             "Mul_NBit_CarryOut_#{Z(@@call_count)}_#{Z(i)}",
                             @n*2)
            },
            (0...@n).collect{|i|
                [
                    "-<#{@result}_#{Z(i)}>  <Mul_NBit_Accum2_#{Z(@@call_count)}_#{Z(@n)}_#{Z(i)}> 0 ",
                    " <#{@result}_#{Z(i)}> -<Mul_NBit_Accum2_#{Z(@@call_count)}_#{Z(@n)}_#{Z(i)}> 0 "
                ]
            },
            
            "-<#{@over_flow}> "+
            (0...@n).collect{|i|
                " <Mul_NBit_Accum2_#{Z(@@call_count)}_#{Z(@n)}_#{Z(i+@n)}> "
            }.join(" ")+" 0 ",
            (0...@n).collect{|i|
                "<#{@over_flow}>  -<Mul_NBit_Accum2_#{Z(@@call_count)}_#{Z(@n)}_#{Z(i+@n)}> 0 "
            }
        ].flatten
    end
end


#
#  in_a * in_b == result
#
class Mul_NBit_1Bit
    attr_accessor :in_a
    attr_accessor :in_b
    attr_accessor :result
    attr_accessor :n

    @@call_count = 0

    def initialize(_in_a,_in_b,_result,_shift,_n)
        @in_a        = _in_a
        @in_b        = _in_b
        @result      = _result
        @n           = _n
    end

    def expand()
        [
            (0...@n).collect{|i|
                [
                    " <#{result}_#{Z(i)}> -<#{@in_a}_#{Z(i)}> -<#{in_b}> 0 ",
                    "-<#{result}_#{Z(i)}> -<#{@in_a}_#{Z(i)}>  <#{in_b}> 0 ",
                    "-<#{result}_#{Z(i)}>  <#{@in_a}_#{Z(i)}> -<#{in_b}> 0 ",
                    "-<#{result}_#{Z(i)}>  <#{@in_a}_#{Z(i)}>  <#{in_b}> 0 ",
                ]
            }
        ].flatten
    
    end
end

#
# in_a & in_b == result
#
class And_1Bit
    attr_accessor :in_a
    attr_accessor :in_b
    attr_accessor :result

    @@call_count=0

    def initialize(_in_a,_in_b,_result)
        @in_a   = _in_a
        @in_b   = _in_b
        @result = _result
    end

    def expand()
       [
            " <#{@in_a}>  <#{@in_b}> -<#{@result}> 0 ",
            " <#{@in_a}> -<#{@in_b}> -<#{@result}> 0 ",
            "-<#{@in_a}>  <#{@in_b}> -<#{@result}> 0 ",
            "-<#{@in_a}> -<#{@in_b}>  <#{@result}> 0 "
       ] 
    end
end  

#
# result == ( in_a < in_b )
#
class LessThan_1Bit
    attr_accessor :in_a
    attr_accessor :in_b
    attr_accessor :result

    @@call_count=0

    def initialize(_in_a,_in_b,_result)
        @in_a   = _in_a
        @in_b   = _in_b
        @result = _result
    end

    def expand()
       [
            " <#{@in_a}>  <#{@in_b}> -<#{@result}> 0 ",
            " <#{@in_a}> -<#{@in_b}>  <#{@result}> 0 ",
            "-<#{@in_a}>  <#{@in_b}> -<#{@result}> 0 ",
            "-<#{@in_a}> -<#{@in_b}> -<#{@result}> 0 "
       ] 
    end
end  


#
# result == ( in_a == in_b )
#
class Equals_1Bit
    attr_accessor :in_a
    attr_accessor :in_b
    attr_accessor :result

    @@call_count=0

    def initialize(_in_a,_in_b,_result)
        @in_a   = _in_a
        @in_b   = _in_b
        @result = _result
    end

    def expand()
       [
            " <#{@in_a}>  <#{@in_b}>  <#{@result}> 0 ",
            " <#{@in_a}> -<#{@in_b}> -<#{@result}> 0 ",
            "-<#{@in_a}>  <#{@in_b}> -<#{@result}> 0 ",
            "-<#{@in_a}> -<#{@in_b}>  <#{@result}> 0 "
       ] 
    end
end  

#
# in_a == in_b
#
class Equals_NBit
    attr_accessor :in_a
    attr_accessor :in_b
    attr_accessor :_n

    @@call_count=0

    def initialize(_in_a,_in_b,_n)
        @in_a  = _in_a
        @in_b  = _in_b
        @n     = _n
    end

    def expand()
        (0...@n).collect{|i|
            [
                "-<#{@in_a}_#{Z(i)}>  <#{@in_b}_#{Z(i)}> 0 ",
                " <#{@in_a}_#{Z(i)}> -<#{@in_b}_#{Z(i)}> 0 "
            ]   
        }.flatten
    end
end  

#
# in_a < in_b
#
class LessThan_NBit
    attr_accessor :in_a
    attr_accessor :in_b
    attr_accessor :n

    @@call_count=0

    def initialize(_in_a,_in_b,_n)
        @in_a   = _in_a
        @in_b   = _in_b
        @n      = _n
    end

    def expand()
        @@call_count+=1
        [
            (0...@n).collect{|i|
                Equals_1Bit.new("#{@in_a}_#{Z(i)}","#{@in_b}_#{Z(i)}",
                                "LessThan_NBit_Equals_#{Z(@@call_count)}_#{Z(i)}")
            },
            (0...@n).collect{|i|
                LessThan_1Bit.new("#{@in_a}_#{Z(i)}",
                                  "#{@in_b}_#{Z(i)}",
                                  "LessThan_NBit_Less_#{Z(@@call_count)}_#{Z(i)}")            
            },
            "<LessThan_NBit_EqualAccum_#{Z(@@call_count)}_#{Z(@n)}> 0 ",
            (0...@n).collect{|i|
                And_1Bit.new("LessThan_NBit_EqualAccum_#{Z(@@call_count)}_#{Z(i+1)}",
                             "LessThan_NBit_Equals_#{Z(@@call_count)}_#{Z(i)}",
                             "LessThan_NBit_EqualAccum_#{Z(@@call_count)}_#{Z(i)}")
            },
            (0...@n).collect{|i|
                And_1Bit.new("LessThan_NBit_EqualAccum_#{Z(@@call_count)}_#{Z(i+1)}",
                             "LessThan_NBit_Less_#{Z(@@call_count)}_#{Z(i)}",
                             "LessThan_NBit_Result_#{Z(@@call_count)}_#{Z(i)}")
            },
            (0...@n).inject(""){|clause,i|
                clause + " <LessThan_NBit_Result_#{Z(@@call_count)}_#{Z(i)}> "
            }+ " 0 "
        ].flatten
    end

end

#
# in_a == in_b * div + mod
#
class DivMod_NBit
    attr_accessor :in_a
    attr_accessor :in_b
    attr_accessor :div 
    attr_accessor :mod
    attr_accessor :n

    @@call_count = 0
  
    def initialize(_in_a,_in_b,_div,_mod,_n)
        @in_a = _in_a
        @in_b = _in_b
        @div  = _div
        @mod  = _mod
        @n    = _n
    end

    def expand()
        @@call_count+=1

        [
            Mul_NBit.new(@in_b,@div,"DivMod_NBit_Accum_#{Z(@@call_count)}","DivMode_NBit_MulOverflow_#{Z(@@call_count)}",@n),
            Add_NBit.new("DivMod_NBit_Accum_#{Z(@@call_count)}",@mod,@in_a,"DivMode_NBit_AddOverflow_#{Z(@@call_count)}",@n),
            "-<DivMode_NBit_MulOverflow_#{Z(@@call_count)}> 0 ",
            "-<DivMode_NBit_AddOverflow_#{Z(@@call_count)}> 0 ",
            LessThan_NBit.new(@mod,@in_b,@n)
        ]
    end

end

#
# result == if cond then a else b end
#
class If_Cond_A_Else_B_1Bit
    attr_accessor :in_a
    attr_accessor :in_b
    attr_accessor :cond 
    attr_accessor :result

    @@call_count = 0
  
    def initialize(_in_a,_in_b,_cond,_result)
        @in_a    = _in_a
        @in_b    = _in_b
        @cond    = _cond
        @result  = _result
    end

    def expand()
        [
            "-<#{@cond}> -<#{@in_a}>  <#{@result}> 0 ",
            "-<#{@cond}>  <#{@in_a}> -<#{@result}> 0 ",
            " <#{@cond}> -<#{@in_b}>  <#{@result}> 0 ",
            " <#{@cond}>  <#{@in_b}> -<#{@result}> 0 "
        ]
    end
end

#
# result == if cond then a else b end
#
class If_Cond_A_Else_B_NBit
    attr_accessor :in_a
    attr_accessor :in_b
    attr_accessor :cond 
    attr_accessor :result
    attr_accessor :n

    @@call_count = 0
  
    def initialize(_in_a,_in_b,_cond,_result,_n)
        @in_a    = _in_a
        @in_b    = _in_b
        @cond    = _cond
        @result  = _result
        @n       = _n
    end

    def expand()
        (0...@n).collect{|i|
            If_Cond_A_Else_B_1Bit.new("#{@in_a}_#{Z(i)}",
                                      "#{@in_b}_#{Z(i)}",
                                      @cond,
                                      "#{@result}_#{Z(i)}")
        }
    end
end


#
# result == in_a | in_b
#
class Or_1Bit
    attr_accessor :in_a
    attr_accessor :in_b
    attr_accessor :result

    def initialize(_in_a,_in_b,_result)
        @in_a        = _in_a
        @in_b        = _in_b
        @result      = _result
    end

    def expand()
        [
            "-<#{@in_a}> -<#{@in_b}>  <#{@result}> 0 ",
            "-<#{@in_a}>  <#{@in_b}>  <#{@result}> 0 ",
            " <#{@in_a}> -<#{@in_b}>  <#{@result}> 0 ",
            " <#{@in_a}>  <#{@in_b}> -<#{@result}> 0 "
        ]
    end
end

#
# result == in_a_1 | in_a_2 | ... | in_a_n
#
class Or_NBit_To_1Bit
    attr_accessor :in_a
    attr_accessor :result
    attr_accessor :n

    def initialize(_in_a,_result,_n)
        @in_a        = _in_a
        @result      = _result
        @n           = _n
    end

    def expand()
        [
            "-<#{@result}> "+
            (0...@n).collect{|i|
                " <#{@in_a}_#{Z(i)}> "
            }.join(" ")+" 0 ",
            (0...@n).collect{|i|
                "<#{@result}> -<#{@in_a}_#{Z(i)}> 0 "
            }
        ].flatten
    end

end


#
# result == in_a ** in_b
#
class Pow_NBit
    attr_accessor :in_a
    attr_accessor :in_b
    attr_accessor :result
    attr_accessor :over_flow
    attr_accessor :n

    @@call_count = 0

    def initialize(_in_a,_in_b,_result,_over_flow,_n)
        @in_a        = _in_a
        @in_b        = _in_b
        @result      = _result
        @over_flow   = _over_flow
        @n           = _n
    end

    def expand()
        @@call_count+=1
        [
            Equals_NBit.new("Pow_NBit_Temp1_#{Z(@@call_count)}_#{Z(0)}",@in_a,@n),
            (0...@n).collect{|i|
                Mul_NBit.new("Pow_NBit_Temp1_#{Z(@@call_count)}_#{Z(i)}",
                             "Pow_NBit_Temp1_#{Z(@@call_count)}_#{Z(i)}",
                             "Pow_NBit_Temp1_#{Z(@@call_count)}_#{Z(i+1)}",
                             "Pow_NBit_Temp1Overflow_#{Z(@@call_count)}_#{Z(i)}",
                             @n)
            },
            (0...@n).collect{|i|
                If_Cond_A_Else_B_NBit.new("Pow_NBit_Temp1_#{Z(@@call_count)}_#{Z(i)}",
                                          "One_NBit_#{Z(@n)}",
                                          "#{@in_b}_#{Z(i)}",
                                          "Pow_NBit_Temp2_#{Z(@@call_count)}_#{Z(i)}",
                                          @n)
            },
            Input_Equals_Number.new("Pow_NBit_PowAccum_#{Z(@@call_count)}_#{Z(0)}",1,@n),
            (0...@n).collect{|i|
                Mul_NBit.new("Pow_NBit_Temp2_#{Z(@@call_count)}_#{Z(i)}",
                             "Pow_NBit_PowAccum_#{Z(@@call_count)}_#{Z(i)}",
                             "Pow_NBit_PowAccum_#{Z(@@call_count)}_#{Z(i+1)}",
                             "Pow_NBit_PowAccumOverflow_#{Z(@@call_count)}_#{Z(i)}",
                             @n)
            },
            Equals_NBit.new(@result,"Pow_NBit_PowAccum_#{Z(@@call_count)}_#{Z(@n)}",@n),
            "-<Pow_NBit_PowAccumOverflowAccum_#{Z(@@call_count)}_#{Z(0)}> 0 ",
            (0...@n).collect{|i|
                Or_1Bit.new("Pow_NBit_PowAccumOverflowAccum_#{Z(@@call_count)}_#{Z(i)}",
                            "Pow_NBit_Temp1Overflow_#{Z(@@call_count)}_#{Z(i)}",
                            "Pow_NBit_PowAccumOverflowAccum_#{Z(@@call_count)}_#{Z(i+1)}")
            },
            (0...@n).collect{|i|
                If_Cond_A_Else_B_1Bit.new("Pow_NBit_PowAccumOverflowAccum_#{Z(@@call_count)}_#{Z(i+1)}",
                                          "Zero_1Bit_#{Z(1)}",
                                          "#{@in_b}_#{Z(i+1)}",
                                          "Pow_NBit_OverflowTemp_#{Z(@@call_count)}_#{Z(i)}"
                                          )
            },
            Or_NBit_To_1Bit.new("Pow_NBit_PowAccumOverflow_#{Z(@@call_count)}",
                                "Pow_NBit_PowAccumOverflow_OR_#{Z(@@call_count)}",
                                @n),
            Or_NBit_To_1Bit.new("Pow_NBit_OverflowTemp_#{Z(@@call_count)}",
                                "Pow_NBit_OverflowTemp_OR_#{Z(@@call_count)}",
                                @n),
            Or_1Bit.new("Pow_NBit_PowAccumOverflow_OR_#{Z(@@call_count)}",
                        "Pow_NBit_OverflowTemp_OR_#{Z(@@call_count)}",
                        @over_flow)

        ].flatten
    end
end


#
# result[0...n] == in_a
# result[n...(2*n)] == 0
#
class DoubleSize_Assign
    attr_accessor :in_a
    attr_accessor :result
    attr_accessor :n

    def initialize(_in_a,_result,_n)
        @in_a        = _in_a
        @result      = _result
        @n           = _n
    end

    def expand()
        [
            Equals_NBit.new(@in_a,@result,@n),
            (@n...(@n*2)).collect{|i| "-<#{@result}_#{Z(i)}> 0 "}
        ].flatten
    end
end    


#
# result == ( base ** exp ) % mod
#
class PowMod_NBit
    attr_accessor :base
    attr_accessor :exp
    attr_accessor :mod
    attr_accessor :result
    attr_accessor :n

    @@call_count = 0

    def initialize(_base,_exp,_mod,_result,_n)
        @base        = _base
        @exp         = _exp
        @mod         = _mod
        @result      = _result
        @n           = _n
    end

    def expand()
        @@call_count+=1
        [
            DoubleSize_Assign.new(@base,"PowMod_NBit_Base_DoubleSize_#{Z(@@call_count)}",@n),
            DoubleSize_Assign.new(@exp, "PowMod_NBit_Exp_DoubleSize_#{Z(@@call_count)}",@n),
            DoubleSize_Assign.new(@mod, "PowMod_NBit_Mod_DoubleSize_#{Z(@@call_count)}",@n),

            Input_Equals_Number.new("PowMod_NBit_PartialResult_#{Z(@@call_count)}_#{Z(0)}",
                                    1,
                                    @n*2), #partial_result_0 = 1
            Equals_NBit.new("PowMod_NBit_CurrentPow_#{Z(@@call_count)}_#{Z(0)}",
                            "PowMod_NBit_Base_DoubleSize_#{Z(@@call_count)}",
                            @n*2), # current_pow_0 = base
            (0...@n).collect{|i|
                If_Cond_A_Else_B_NBit.new("PowMod_NBit_CurrentPow_#{Z(@@call_count)}_#{Z(i)}",
                                          "One_NBit_#{Z(@n*2)}",
                                          "PowMod_NBit_Exp_DoubleSize_#{Z(@@call_count)}_#{Z(i)}",
                                          "PowMod_NBit_BitFactor_#{Z(@@call_count)}_#{Z(i)}",
                                          @n*2
                                          ) # bit_factor_i = if exp_i current_pow else 1
            },
            (0...@n).collect{|i|
                Mul_NBit.new("PowMod_NBit_PartialResult_#{Z(@@call_count)}_#{Z(i)}",
                             "PowMod_NBit_BitFactor_#{Z(@@call_count)}_#{Z(i)}",
                             "PowMod_NBit_Multipled_#{Z(@@call_count)}_#{Z(i)}",
                             "PowMod_NBit_MultipledOverflow_#{Z(@@call_count)}_#{Z(i)}",
                             @n*2
                ) # multipled_i = partial_result * bit_factor_i
            },
            (0...@n).collect{|i|
                DivMod_NBit.new("PowMod_NBit_Multipled_#{Z(@@call_count)}_#{Z(i)}",
                                "PowMod_NBit_Mod_DoubleSize_#{Z(@@call_count)}",
                                "PowMod_NBit_Div1_#{Z(@@call_count)}_#{Z(i)}",
                                "PowMod_NBit_PartialResult_#{Z(@@call_count)}_#{Z(i+1)}",
                                @n*2
                )#partial_result_(i+1) = multipled_i % mod
            },
            (0...@n).collect{|i|
                Mul_NBit.new("PowMod_NBit_CurrentPow_#{Z(@@call_count)}_#{Z(i)}",
                             "PowMod_NBit_CurrentPow_#{Z(@@call_count)}_#{Z(i)}",
                             "PowMod_NBit_SquareBase_#{Z(@@call_count)}_#{Z(i)}",
                             "PowMod_NBit_SquareBaseOverflow_#{Z(@@call_count)}_#{Z(i)}",
                             @n*2
                ) #square_base_i = current_pow_i * current_i
            },
            (0...@n).collect{|i|
                DivMod_NBit.new("PowMod_NBit_SquareBase_#{Z(@@call_count)}_#{Z(i)}",
                                "PowMod_NBit_Mod_DoubleSize_#{Z(@@call_count)}",
                                "PowMod_NBit_Div2_#{Z(@@call_count)}_#{Z(i)}",
                                "PowMod_NBit_CurrentPow_#{Z(@@call_count)}_#{Z(i+1)}",
                                @n*2
                )# current_pow_(i+1) = square_base_i % mod
            },
            Equals_NBit.new(@result,
                            "PowMod_NBit_PartialResult_#{Z(@@call_count)}_#{Z(@n)}",
                            @n) # result = partial_result_n
        ].flatten
    end

end

#
# target is a composite number.
#
class IsComposite
    attr_accessor :target
    attr_accessor :n

    @@call_count=0

    def initialize(_target,_n)
        @target = _target
        @n      = _n
    end

    def expand()
        @@call_count+=1
        [
            Mul_NBit.new("IsComposite_fact1_#{Z(@@call_count)}",
                         "IsComposite_fact2_#{Z(@@call_count)}",
                         @target,
                         "IsComposite_Overflow_#{Z(@@call_count)}",
                         @n),
            Input_Not_Equals_Number.new("IsComposite_fact1_#{Z(@@call_count)}",0,@n),
            Input_Not_Equals_Number.new("IsComposite_fact2_#{Z(@@call_count)}",0,@n),
            Input_Not_Equals_Number.new("IsComposite_fact1_#{Z(@@call_count)}",1,@n),
            Input_Not_Equals_Number.new("IsComposite_fact2_#{Z(@@call_count)}",1,@n),
            "-<IsComposite_Overflow_#{Z(@@call_count)}> 0 "
       ]
    end

end


#
# make clause ( literal & condition )
#
class AddLiteralToCondition
    attr_accessor :literal
    attr_accessor :condition

    def initialize(_literal,_condition)
        @literal=_literal
        @condition=_condition
    end

    def expand()
        c=true
        cond=[@condition]
        while(c)
            c=false
            cond=cond.collect{|x| 
                if x.respond_to?(:expand) then
                    c=true
                    x.expand
                else
                    x 
                end
             }.flatten
        end
        cond.collect{|c| " #{@literal}  #{c} "}
    end
end


#
# condition1 || condition2
#
class Or_Condition
    attr_accessor :condition1
    attr_accessor :condition2

    @@call_count=0

    def initialize(_condition1,_condition2)
        @condition1=_condition1
        @condition2=_condition2
    end

    def expand()
        @@call_count+=1

        [
            AddLiteralToCondition.new("  <Or_Condition_#{Z(@@call_count)}>",@condition1),
            AddLiteralToCondition.new(" -<Or_Condition_#{Z(@@call_count)}>",@condition2),            
        ].flatten 
    end

end

#
# condition1 && condition2
#
class And_Condition
    attr_accessor :condition1
    attr_accessor :condition2

    @@call_count=0

    def initialize(_condition1,_condition2)
        @condition1=_condition1
        @condition2=_condition2
    end

    def expand()
        @@call_count+=1

        [
            @condition1,
            @condition2            
        ].flatten 
    end

end

#
# output == input_1 + input_2 + ... + input_(data_count)
#
class Sum_NBit
    attr_accessor :input
    attr_accessor :output
    attr_accessor :overflow
    attr_accessor :data_count
    attr_accessor :bits

    @@call_count = 0

    def initialize(_input,_output,_overflow,_data_count,_bits)
        @input       = _input
        @output      = _output
        @overflow    = _overflow
        @data_count  = _data_count
        @bits        = _bits
    end

    def expand()
        @@call_count+=1
        [
            Input_Equals_Number.new("Sum_NBit_Accum_#{Z(@@call_count)}_#{Z(0)}",0,@bits),
            (0...@data_count).collect{|i|
                Add_NBit.new("#{@input}_#{Z(i)}",
                             "Sum_NBit_Accum_#{Z(@@call_count)}_#{Z(i)}",
                             "Sum_NBit_Accum_#{Z(@@call_count)}_#{Z(i+1)}",
                             "Sum_NBit_Overflow_#{Z(@@call_count)}_#{Z(i)}",
                             @bits)
            },
            Equals_NBit.new(@output,
                            "Sum_NBit_Accum_#{Z(@@call_count)}_#{Z(@data_count)}",
                            @bits),
            Or_NBit_To_1Bit.new("Sum_NBit_Overflow_#{Z(@@call_count)}",
                                @overflow,
                                @data_count)
        ].flatten
    end
end

#
# output == input_1 * input_2 * ... * input_(data_count) 
#
class Product_NBit
    attr_accessor :input
    attr_accessor :output
    attr_accessor :overflow
    attr_accessor :data_count
    attr_accessor :bits

    @@call_count = 0

    def initialize(_input,_output,_overflow,_data_count,_bits)
        @input       = _input
        @output      = _output
        @overflow    = _overflow
        @data_count  = _data_count
        @bits        = _bits
    end

    def expand()
        @@call_count+=1
        [
            Input_Equals_Number.new("Product_NBit_Accum_#{Z(@@call_count)}_#{Z(0)}",1,@bits),
            (0...@data_count).collect{|i|
                Mul_NBit.new("#{@input}_#{Z(i)}",
                             "Product_NBit_Accum_#{Z(@@call_count)}_#{Z(i)}",
                             "Product_NBit_Accum_#{Z(@@call_count)}_#{Z(i+1)}",
                             "Product_NBit_Overflow_#{Z(@@call_count)}_#{Z(i)}",
                             @bits)
            },
            Equals_NBit.new(@output,
                            "Product_NBit_Accum_#{Z(@@call_count)}_#{Z(@data_count)}",
                            @bits),
            Or_NBit_To_1Bit.new("Product_NBit_Overflow_#{Z(@@call_count)}",
                                @overflow,
                                @data_count)
        ].flatten
    end
end


#
# ( generator ** pow ) % mod == 1
#
class FermatTest
    attr_accessor :generator
    attr_accessor :pow
    attr_accessor :mod
    attr_accessor :n

    @@call_count=0

    def initialize(_generator,_pow,_mod,_n)
        @generator=_generator
        @pow = _pow
        @mod = _mod
        @n   = _n
    end

    def expand()
        @@call_count+=1
        [
            Input_Not_Equals_Number.new(@generator,0,@n),
            Input_Not_Equals_Number.new(@generator,1,@n),
            PowMod_NBit.new(@generator,
                            @pow,
                            @mod,
                            "FermatTest_#{Z(@@call_count)}",
                            @n),
            Input_Equals_Number.new("FermatTest_#{Z(@@call_count)}",
                                    1,
                                    @n)
        ]

    end
end

#
# (generator ** (prime-1)) % prime == 1
#
class FermatTest2
    attr_accessor :generator
    attr_accessor :prime
    attr_accessor :n

    @@call_count=0

    def initialize(_generator,_prime,_n)
        @generator=_generator
        @prime = _prime
        @n   = _n
    end

    def expand()
        @@call_count+=1
        [
            Add_NBit.new("FermatTest2_Prime_Minus1_#{Z(@@call_count)}",
                         "One_NBit_#{Z(@n)}",
                         @prime,
                         "FermatTest2_Prime_Minus1_Overflow_#{Z(@@call_count)}",
                         @n),
            "-<FermatTest2_Prime_Minus1_Overflow_#{Z(@@call_count)}> 0 ",
            FermatTest.new(@generator,
                           "FermatTest2_Prime_Minus1_#{Z(@@call_count)}",
                            @prime,
                            @n
                          )
        ]
    end
end

#
# ( generator ** pow ) % mod != 1
#
class FermatTest3
    attr_accessor :generator
    attr_accessor :pow
    attr_accessor :mod
    attr_accessor :n

    @@call_count=0

    def initialize(_generator,_pow,_mod,_n)
        @generator=_generator
        @pow = _pow
        @mod = _mod
        @n   = _n
    end

    def expand()
        @@call_count+=1
        [
            Input_Not_Equals_Number.new(@generator,0,@n),
            Input_Not_Equals_Number.new(@generator,1,@n),
            PowMod_NBit.new(@generator,
                            @pow,
                            @mod,
                            "FermatTest3_#{Z(@@call_count)}",
                            @n),
            Input_Not_Equals_Number.new("FermatTest3_#{Z(@@call_count)}",
                                    1,
                                    @n)
        ]

    end
end

#
# target is a prime number.
#
class IsPrime
    attr_accessor :target
    attr_accessor :n
    attr_accessor :num_prime

    @@call_count=0

    def initialize(_target,_n,_num_prime=_n)
        @target     = _target
        @n          = _n
        @num_prime  = _num_prime
    end

    def expand()
        @@call_count+=1
        [
            (0...@num_prime).collect{|i|
                Input_Not_Equals_Number.new("IsPrime_Prime_#{Z(@@call_count)}_#{Z(i)}",0,@n) # prime[i]!=0
            },
            (0...@num_prime).collect{|i|
                Input_Not_Equals_Number.new("IsPrime_Prime_#{Z(@@call_count)}_#{Z(i)}",1,@n) # prime[i]!=1
            },
            (0...@num_prime).collect{|i|
                (0...@num_prime).collect{|j|
                    Pow_NBit.new("IsPrime_Prime_#{Z(@@call_count)}_#{Z(j)}",
                                 "IsPrime_Pow_#{Z(@@call_count)}_#{Z(i)}_#{Z(j)}",
                                 "IsPrime_PowTemp_#{Z(@@call_count)}_#{Z(i)}_#{Z(j)}",
                                 "IsPrime_PowTemp_Overflow_#{Z(@@call_count)}_#{Z(i)}_#{Z(j)}",
                                 @n) # pow_temp[i][j] = pow(prime[j],pow[i][j])
                }
            },
            (0...@num_prime).collect{|i|
                (0...@num_prime).collect{|j|
                    "-<IsPrime_PowTemp_Overflow_#{Z(@@call_count)}_#{Z(i)}_#{Z(j)}> 0 " # powtemp_overflow = 0
                }
            },

            (0...@num_prime).collect{|i|
                Product_NBit.new("IsPrime_PowTemp_#{Z(@@call_count)}_#{Z(i)}",
                                 "IsPrime_Product_#{Z(@@call_count)}_#{Z(i)}",
                                 "IsPrime_Product_Overflow_#{Z(@@call_count)}_#{Z(i)}",
                                 @num_prime,
                                 @n)#product[i]= product j (pow_temp[i][j])
            },
            (0...@num_prime).collect{|i|
                "-<IsPrime_Product_Overflow_#{Z(@@call_count)}_#{Z(i)}> 0 " # product_overflow = 0
            },
            (0...@num_prime).collect{|i|
                Add_NBit.new("IsPrime_Product_#{Z(@@call_count)}_#{Z(i)}",
                             "One_NBit_#{Z(@n)}",
                             "IsPrime_Product_Plus1_#{Z(@@call_count)}_#{Z(i)}",
                             "IsPrime_Product_Plus1_Overflow_#{Z(@@call_count)}_#{Z(i)}",
                              @n) # product_plus1[i] = product[i] + 1
            },
            (0...@num_prime).collect{|i|
                "-<IsPrime_Product_Plus1_Overflow_#{Z(@@call_count)}_#{Z(i)}> 0 " # product_plus1_overflow[i] = 0 
            },
            (0...@num_prime).collect{|i|
                Sum_NBit.new("IsPrime_Pow_#{Z(@@call_count)}_#{Z(i)}",
                             "IsPrime_SumPow_#{Z(@@call_count)}_#{Z(i)}",
                             "IsPrime_SumPow_Overflow_#{Z(@@call_count)}_#{Z(i)}",
                             @num_prime,
                             @n)# sumpow[i]= sum j pow[i][j]
            },
            (0...@num_prime).collect{|i|
                "-<IsPrime_SumPow_Overflow_#{Z(@@call_count)}_#{Z(i)}> 0 " # sumpow_overflow[i] = 0
            },

            (0...@num_prime).collect{|i|
                Or_Condition.new(
                    Or_Condition.new(Input_Equals_Number.new("IsPrime_Prime_#{Z(@@call_count)}_#{Z(i)}",2,@n),
                                     Input_Equals_Number.new("IsPrime_Prime_#{Z(@@call_count)}_#{Z(i)}",3,@n)
                    ),
                    And_Condition.new(
                        LessThan_NBit.new("One_NBit_#{Z(@n)}",
                                          "IsPrime_SumPow_#{Z(@@call_count)}_#{Z(i)}",
                                          @n),
                        Equals_NBit.new("IsPrime_Product_Plus1_#{Z(@@call_count)}_#{Z(i)}",
                                        "IsPrime_Prime_#{Z(@@call_count)}_#{Z(i)}",
                                        @n)
                    )
                ) # prime[i]==2 or (1<sumpow[i] and product_plus1[i]==prime[i])
            },
            (0...@num_prime).collect{|i|
                Add_NBit.new("IsPrime_Prime_Minus1_#{Z(@@call_count)}_#{Z(i)}",
                             "One_NBit_#{Z(@n)}",
                             "IsPrime_Prime_#{Z(@@call_count)}_#{Z(i)}",
                             "IsPrime_Prime_Minus1_Overflow_#{Z(@@call_count)}_#{Z(i)}",
                             @n)# prime[i]=prime_minus1[i]+1
            },
            (0...@num_prime).collect{|i|
                "-<IsPrime_Prime_Minus1_Overflow_#{Z(@@call_count)}_#{Z(i)}> 0 "
            },

            (0...@num_prime).collect{|i|
                (0...@num_prime).collect{|j|
                    DivMod_NBit.new("IsPrime_Prime_Minus1_#{Z(@@call_count)}_#{Z(i)}",
                                    "IsPrime_Prime_#{Z(@@call_count)}_#{Z(j)}",
                                    "IsPrime_Div_#{Z(@@call_count)}_#{Z(i)}_#{Z(j)}",
                                    "IsPrime_Mod_#{Z(@@call_count)}_#{Z(i)}_#{Z(j)}",
                                    @n
                                    )#div[i][j]=prime_minus1[i]/prime[i]
                }
            },
            (0...@num_prime).collect{|i|
                (0...@num_prime).collect{|j|
                    Or_Condition.new(
                        Or_Condition.new(
                            FermatTest3.new("IsPrime_Generator_#{Z(@@call_count)}_#{Z(i)}",
                                           "IsPrime_Div_#{Z(@@call_count)}_#{Z(i)}_#{Z(j)}",
                                           "IsPrime_Prime_#{Z(@@call_count)}_#{Z(i)}",
                                            @n),# (generator[i]**div[i])%prime[i] != 1
                            Input_Equals_Number.new("IsPrime_Pow_#{Z(@@call_count)}_#{Z(i)}_#{Z(j)}",
                                                    0,
                                                    @n)#pow[i][j]==0
                        ),
                        Or_Condition.new(
                            Input_Equals_Number.new("IsPrime_Prime_#{Z(@@call_count)}_#{Z(i)}",2,@n),        
                            Input_Equals_Number.new("IsPrime_Prime_#{Z(@@call_count)}_#{Z(i)}",3,@n)
                        )
                    )
                }
            },      
            (0...@num_prime).collect{|i|
                Or_Condition.new(
                    FermatTest2.new("IsPrime_Generator_#{Z(@@call_count)}_#{Z(i)}",
                                    "IsPrime_Prime_#{Z(@@call_count)}_#{Z(i)}",
                                    @n
                                    ),
                    Or_Condition.new(
                        Input_Equals_Number.new("IsPrime_Prime_#{Z(@@call_count)}_#{Z(i)}",2,@n),
                        Input_Equals_Number.new("IsPrime_Prime_#{Z(@@call_count)}_#{Z(i)}",3,@n)
                    )
                )
            },
            Equals_NBit.new(@target,
                            "IsPrime_Prime_#{Z(@@call_count)}_#{Z(0)}",
                            @n)

        ].flatten

    end
end

def generate_cnf(condition,file_path)

    c=true
    iter=0
    while(c)
        iter+=1
        c=false
        STDERR.print("expand : #{iter}\n")
        condition=condition.collect{|x| 
            if x.respond_to?(:expand) then
                c=true
                x.expand
            else
                x 
            end
         }.flatten
    end

    STDERR.print("gather literals...\n")
    literals=condition.collect.with_index{|clause,i|
        if((i%(condition.length/20))==0)then
            STDERR.print("#{5*i/(condition.length/20)}%...\n")
        end
        clause.scan(/<[a-zA-Z0-9_]+>/)
    }.flatten

    STDERR.print("sorting literals...\n")
    literals=literals.sort{|a,b| 
        if (a=~/^<[A-Z]/ &&  b=~/^<[A-Z]/) || (a=~/^<[a-z]/ &&  b=~/^<[a-z]/)
            a <=> b
        elsif (a=~/^<[A-Z]/ &&  b=~/^<[a-z]/)
            1
        else
            -1
            end
    }

    STDERR.print("uniqing literal...\n")
    literals=literals.uniq

    literal_map={}

    STDERR.print("mapping symbol to integer...\n")
    literals.each_with_index{|literal,i| 
        literal_map[literal]=i+1
    }

    STDERR.print("replacing symbol to integer...\n")
    iter=0
    replaced=condition.collect{|clause|
        iter+=1
        if(iter%(condition.length/20)==0) then
            STDERR.print("#{5*iter/(condition.length/20)}%...\n")
        end
        clause.scan(/<[a-zA-Z0-9_]+>/).inject(clause){|c,literal|
            c.gsub(literal,literal_map[literal].to_s)
        }
    }

    STDERR.print("writing cnf to file...\n")

    open(file_path,"w"){|f|

        f.print "c\n"
        f.print "c\n"
        f.print "c\n"
        literal_map.each{|k,v|
            f.print "cv #{k} #{v} \n"
        }
        f.print "p cnf #{literal_map.length} #{replaced.length}\n"

        replaced.each.with_index{|clause,i|
            if ((i % (replaced.length / 20))==0 ) then
                STDERR.print("#{5*i/(replaced.length/20)}%...\n")
            end
            f.print clause,"\n"
        }
    }
end
