# Copyright (C) 1996 A. Scottedward Hodel 
#
# This file is part of Octave. 
#
# Octave is free software; you can redistribute it and/or modify it 
# under the terms of the GNU General Public License as published by the 
# Free Software Foundation; either version 2, or (at your option) any 
# later version. 
# 
# Octave is distributed in the hope that it will be useful, but WITHOUT 
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License 
# for more details.
# 
# You should have received a copy of the GNU General Public License 
# along with Octave; see the file COPYING.  If not, write to the Free 
# Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. 
 
function retsys = sysout(sys,opt)
# function sysout(sys[,opt])
# print out a system data structure in desired format
#
# sys: system data structure
# opt: []: primary system form (default)
#      "ss": state space form
#      "tf": transfer function form
#      "zp": zero-pole form
#      "all": all of the above

# Written by A S Hodel: 1995-1996
# $Revision: 2.0.0.2 $

# save for restoring at end of routine
save_val = implicit_str_to_num_ok;
implicit_str_to_num_ok = 1;

if( (nargin < 1) || (nargin > 2) )
  usage("sysout(sys[,opt])");
endif

if(isempty(sys))
  retsys = sys;
  warning("sysout: empty system")
  return;
endif

if(! is_struct(sys))
  disp("sysout: input must be a system structure")
endif

# set up output type array
if( nargin == 1 )
  opt = sysgettype(sys);
else
  if( ! (strcmp(opt,"ss") + strcmp(opt,"tf") + ...
    strcmp(opt,"zp") + strcmp(opt,"all") ) )
    error("opt must be one of [], \"ss\", \"tf\", \"zp\", or \"all\"");
  endif
endif

# now check output for each form:
[nn,nz,mm,pp] = sysdimensions(sys);
if( mm > 0)
  disp("Input(s)")
  disp(outlist(sysgetsignals(sys,"in"),"	"));
else
  disp("Input(s): none");
endif
if (pp > 0)
  disp("Output(s):")
  disp(outlist(sysgetsignals(sys,"out"), ...
	"	",sysgetsignals(sys,"yd")) );
else
  disp("Output(s): none");
endif
if(sysgettsam(sys) > 0)
  disp(["Sampling interval: ",num2str(sysgettsam(sys))]);
  str = "z";
else
  str = "s";
endif

# transfer function form
if( strcmp(opt,"tf") + strcmp(opt,"all") )
  sys = sysupdate(sys,"tf");		#make sure tf is up to date
  disp("transfer function form:")
  [num,den] = sys2tf(sys);
  tfout(num,den,str);
endif

if( strcmp(opt,"zp") + strcmp(opt,"all") )
  sys = sysupdate(sys,"zp");		#make sure zp is up to date
  disp("zero-pole form:")
  [zer,pol,kk] = sys2zp(sys);
  zpout(zer, pol, kk,str)
endif

if( strcmp(opt,"ss") + strcmp(opt,"all") )
  sys = sysupdate(sys,"ss");
  disp("state-space form:");
  disp([num2str(nn)," continuous states, ", num2str(nz)," discrete states"]);
  if( nn+nz > 0)
    disp("State(s):")
    xi = (nn+1):(nn+nz);
    xd = zeros(1,nn+nz);
    if(!isempty(xi))
      xd(xi) = 1;
    endif
    disp(outlist(sysgetsignals(sys,"st"),"	",xd));
  else
    disp("State(s): none");
  endif

  # display matrix values?
  dmat = (max( [ (nn+nz), mm, pp ] ) <= 32);

  printf("A matrix: %d x %d\n",nn,nn);
  [aa,bb,cc,dd] = sys2ss(sys);
  if(dmat) 	disp(aa); 	endif

  printf("B matrix: %d x %d\n",nn,mm);
  if(dmat)     disp(bb);              endif

  printf("C matrix: %d x %d\n",pp,nn);
  if(dmat) disp(cc);		endif

  printf("D matrix: %d x %d\n",pp,nn);
  if(dmat)       disp(dd);         endif
endif

if(nargout >= 1)
  retsys = sys;
endif 
  
# restore global variable
implicit_str_to_num_ok = save_val;

endfunction
