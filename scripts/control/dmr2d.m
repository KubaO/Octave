# Copyright (C) 1998 A. S. Hodel
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

function [dsys,fidx] = dmr2d (sys, idx, sprefix, Ts2,cuflg)

# Usage: [dsys,fidx] = dmr2d (sys, idx, sprefix, Ts2 {,cuflg})
# convert a multirate digital system to a single rate digital system
# states specified by idx, sprefix are sampled at Ts2, all others
# are sampled at Ts1 = sysgettsam(sys).
# inputs:
#   sys: discrete time system;
#        dmr2d exits with an error if sys is not discrete
#   idx: list of states with sampling time sys.tsam (may be empty)
#   sprefix: string prefix of states with sampling time sys.tsam (may be empty)
#   Ts2: sampling time of states not specified by idx, sprefix
#        must be an integer multiple of sys.tsam
#   cuflg: "constant u flag" if cuflg is nonzero then the system inputs are 
#        assumed to be constant over the revised sampling interval Ts2.
#        Otherwise, since the inputs can change during the interval
#        t in [k Ts2, (k+1) Ts2], an additional set of inputs is
#        included in the revised B matrix so that these intersample inputs
#        may be included in the single-rate system.
#        default: cuflg = 1.
#
# outputs: 
#   dsys: equivalent discrete time system with sampling time Ts2.
#
#         The sampling time of sys is updated to Ts2.
#
#         if cuflg=0 then a set of additional inputs is added to
#         the system with suffixes _d1, ..., _dn to indicate their
#         delay from the starting time k Ts2, i.e.
#         u = [u_1; u_1_d1; ..., u_1_dn] where u_1_dk is the input
#             k*Ts1 units of time after u_1 is sampled. (Ts1 is
#             the original sampling time of discrete time sys and
#             Ts2 = (n+1)*Ts1)
#
#   fidx: indices of "formerly fast" states specified by idx and sprefix;
#         these states are updated to the new slower) sampling interval
#
#
#  WARNING: Not thoroughly tested yet; especially when cuflg == 0.

# Adapted from c2d by a.s.hodel@eng.auburn.edu
# $Log: dmr2d.m,v $
# Revision 1.1  1998/07/21 14:40:55  hodelas
# First attempt at multirate systems analysis
#
# Revision 1.2  1998/07/21 14:13:44  hodel
# squaring A  to get a11^nstp
#
# Revision 1.1  1998/07/21 12:41:46  hodel
# Initial revision
#
#
  save_val = implicit_str_to_num_ok;	# save for later
  implicit_str_to_num_ok = 1;

  # parse input arguments
  if(nargin != 4 | nargout > 2)
    usage("[dsys,fidx] = dmr2d (sys, idx, sprefix, Ts2 {,cuflg})");

  elseif (!is_struct(sys))
    error("sys must be in system data structure form");

  elseif(!is_digital(sys))
    error("sys must be discrete-time; continuous time passed");

  elseif (!(is_vector(idx) | isempty(idx)))
    error(["idx(",num2str(rows(idx)),"x",num2str(columns(idx)), ...
      ") must be a vector"]);

  elseif (any(idx <= 0))
    idv = find(idx <= 0);
    ii = idv(1);
    error(["idx(",num2str(ii),")=",num2str(idx(ii)), ...
      "; entries of idx must be positive"]);

  elseif(!(isstr(sprefix) | isempty(sprefix)))
    error("sprefix must be a string or empty");

  elseif(!is_sample(Ts2))
    error(["Ts2=",num2str(Ts2),"; invalid sampling time"]);

  endif

  # optional argument: cuflg
  if(nargin <= 4)
    cuflg = 1;		# default: constant inputs over Ts2 sampling interv.
  elseif( !is_scalar(cuflg) )
    error("cuflg must be a scalar")
  elseif( cuflg != 0 | cuflg != 1)
    error(["cuflg = ",num2str(cuflg),", should be 0 or 1"]);
  endif

  # extract  state space information
  [da,db,dc,dd,Ts1,nc,nz,stname,inname,outname,yd] = sys2ss(sys);

  # compute number of steps
  if(Ts1 > Ts2)
    error(["Current sampling time=",num2str(Ts1),"< Ts2=",num2str(Ts2)]);
  endif
  nstp = floor(Ts2/Ts1+0.5);
  if(abs((Ts2 - Ts1*nstp)/Ts1) > 1e-12)
    warning(["dmr2d: Ts1=",num2str(Ts1),", Ts2=",num2str(Ts2), ...
      ", selecting nsteps=",num2str(nstp),"; mismatch"]);
  endif

  if(isempty(sprefix) & isempty(idx))
    warning("both sprefix and idx are empty; returning dsys=sys");
    fidx = [];
    dsys = sys;
    return
  elseif(isempty(sprefix))
    fidx = idx;
  else
    fidx = reshape(idx,1,length(idx));
    # find states whose name begins with any strings in sprefix.
    ns = rows(sprefix);
    for kk=1:ns
      spk = dezero(sprefix(kk,:));  # get next prefix and length
      spl = length(spk);

      # check each state name
      for ii=1:nz
        sti = dezero(stname(ii,:));  # compare spk with this state name
        if(length(sti) >= spl)
          # if the prefix matches and ii isn't already in the list, add ii
          if(strcmp(sti(1:spl),spk) & !any(fidx == ii) ) 
            fidx = sort([fidx,ii]);
          endif
        endif
      endfor
    endfor
  endif

  if(nstp == 0)
    warning("dmr2d: nstp = 0; setting tsam and returning");
    dsys = syschtsam(sys,Ts2);
    return
  elseif(nstp < 0)
    error(["nstp = ", num2str(nstp)," < 0; this shouldn't be!"]);
  endif

  # permute system matrices
  pv = sysreorder(nz,fidx);
  pv = pv(nz:-1:1);          # reverse order to put fast modes in leading block

  # construct inverse permutation
  Inz = eye(nz);
  pvi = (Inz(pv,:)'*[1:nz]')';

  # permute A, B (stname permuted for debugging only)
  da = da(pv,pv);
  db = db(pv,:);
  stname = stname(pv,:);

  # partition A, B:
  lfidx = length(fidx);
  bki = 1:lfidx;
  a11 = da(bki,bki);
  b1 = db(bki,:);

  if(lfidx < nz)
    lfidx1 = lfidx+1;
    bki2 = (lfidx1):nz;
    a12 = da(bki,bki2);
    b2 = db(bki2,:);
  else
    warning("dmr2d: converting entire A,B matrices to new sampling rate");
    lfidx1 = -1;
    bki2 = [];
  endif

  #####################################
  # begin system conversion: nstp steps
  #####################################

  # compute abar_{n-1}*a12 and appropriate b matrix stuff
  a12b = a12;      # running  total of abar_{n-1}*a12
  a12w = a12;      # current a11^n*a12  (start with n = 0)
  if(cuflg)
    b1b = b1;
    b1w = b1;
  else
    # cuflg == 0, need to keep track of intersample inputs too
    nzdx = find(max(abs(b1)) != 0);  # FIXME: check tolerance relative to ||b1||
    b1w = b1(nzdx);
    innamenz = inname(nzdx);
    b1b = b1;                        # initial b1 must match columns in b2
  endif

  ######################################
  # compute a11h = a11^nstp by squaring
  a11h = eye(size(a11));
  p2 = 1;
  a11p2 = a11;        #a11^p2

  nstpw = nstp;       # workspace for computing a11^nstp
  while(nstpw > 0.5)
    oddv = rem(nstpw,2);
    if(oddv)
      a11h = a11h*a11p2;
    endif
    nstpw = (nstpw-oddv)/2;
    if(nstpw > 0.5)
      a11p2 = a11p2*a11p2;    # a11^(next power of 2)
    endif
  endwhile
  
  # FIXME: this part should probably also use squaring, but
  # that would require exponentially growing memory.  What do do?
  for kk=2:nstp
    # update a12 block to sum(a12 + ... + a11^(kk-1)*a12)
    a12w = a11*a12w;
    a12b = a12b + a12w;

    # similar for b1 block (checking for cuflg first!)
    b1w = a11*b1w;
    if(cuflg)
      b1b = b1b + b1w;        # update b1 block just like we did a12
    else
      b1b = [b1b, b1w];       # append new inputs
      newin = strappend(innamenz,["_d",num2str(kk-1)]);
      inname = str2mat(inname,newin);
    endif
  endfor

  # reconstruct system and return
  da(bki,bki) = a11h;
  db(bki,1:columns(b1b)) = b1b;
  if(!isempty(bki2))
    da(bki,bki2) = a12b;
  endif

  da = da(pvi,pvi);
  db = db(pvi,:);
  stname = stname(pvi,:);

  # construct new system and return
  dsys = ss2sys(da,db,dc,dd,Ts2,0,nz,stname,inname,outname,find(yd == 1));

  implicit_str_to_num_ok = save_val;	# restore value

endfunction
