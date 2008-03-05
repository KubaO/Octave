c Copyright (C) 2008  VZLU Prague, a.s., Czech Republic
c 
c Author: Jaroslav Hajek <highegg@gmail.com>
c 
c This source is free software; you can redistribute it and/or modify
c it under the terms of the GNU General Public License as published by
c the Free Software Foundation; either version 2 of the License, or
c (at your option) any later version.
c 
c This program is distributed in the hope that it will be useful,
c but WITHOUT ANY WARRANTY; without even the implied warranty of
c MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
c GNU General Public License for more details.
c 
c You should have received a copy of the GNU General Public License
c along with this software; see the file COPYING.  If not, see
c <http://www.gnu.org/licenses/>.
c 

      subroutine dch1up(n,R,u,w)
c purpose:      given an upper triangular matrix R that is a Cholesky
c               factor of a symmetric positive definite matrix A, i.e.
c               A = R'*R, this subroutine updates R -> R1 so that
c               R1'*R1 = A + u*u'
c               (real version)
c arguments:
c n (in)        the order of matrix R
c R (io)        on entry, the upper triangular matrix R
c               on exit, the updated matrix R1
c u (io)        the vector determining the rank-1 update
c               on exit, u is destroyed.
c w (w)         a workspace vector of size n
c 
c NOTE: the workspace vector is used to store the rotations
c       so that R does not need to be traversed by rows.
c
      integer n
      double precision R(n,n),u(n)
      double precision w(n)
      external dlartg,dlartv
      double precision rr,ui,t
      integer i,j
      
      do i = 1,n
c apply stored rotations, column-wise
        ui = u(i)
        do j = 1,i-1
          t = w(j)*R(j,i) + u(j)*ui
          ui = w(j)*ui - u(j)*R(j,i)
          R(j,i) = t
        end do
c generate next rotation        
        call dlartg(R(i,i),ui,w(i),u(i),rr)
        R(i,i) = rr
      end do
      end

