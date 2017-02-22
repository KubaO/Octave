## Copyright (C) 2013-2017 Jacopo Corno <jacopo.corno@gmail.com>
## Copyright (C) 2013-2016 Roberto Porcu' <roberto.porcu@polimi.it>
##
## This file is part of Octave.
##
## Octave is free software; you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 3 of the License, or
## (at your option) any later version.
##
## Octave is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with Octave; see the file COPYING.  If not, see
## <http://www.gnu.org/licenses/>.

## -*- texinfo -*-
## @deftypefn  {} {[@var{t_next}, @var{x_next}] =} runge_kutta_23 (@var{fun}, @var{t}, @var{x}, @var{dt})
## @deftypefnx {} {[@var{t_next}, @var{x_next}] =} runge_kutta_23 (@var{fun}, @var{t}, @var{x}, @var{dt}, @var{options})
## @deftypefnx {} {[@var{t_next}, @var{x_next}] =} runge_kutta_23 (@var{fun}, @var{t}, @var{x}, @var{dt}, @var{options}, @var{k_vals})
## @deftypefnx {} {[@var{t_next}, @var{x_next}] =} runge_kutta_23 (@var{fun}, @var{t}, @var{x}, @var{dt}, @var{options}, @var{k_vals}, @var{t_next})
## @deftypefnx {} {[@var{t_next}, @var{x_next}, @var{x_est}] =} runge_kutta_23 (@dots{})
## @deftypefnx {} {[@var{t_next}, @var{x_next}, @var{x_est}, @var{k_vals_out}] =} runge_kutta_23 (@dots{})
##
## This function can be used to integrate a system of ODEs with a given initial
## condition @var{x} from @var{t} to @var{t+dt}, with the Bogacki-Shampine
## method of third order.  For the definition of this method see
## @url{http://en.wikipedia.org/wiki/List_of_Runge%E2%80%93Kutta_methods}.
##
## @var{fun} is a function handle that defines the ODE: @code{y' = f(tau,y)}.
## The function must accept two inputs where the first is time @var{tau} and
## the second is a column vector of unknowns @var{y}.
##
## @var{t} is the first extreme of integration interval.
##
## @var{x} is the initial condition of the system..
##
## @var{dt} is the timestep, that is the length of the integration interval.
##
## The optional fourth argument @var{options} specifies options for the ODE
## solver.  It is a structure generated by @code{odeset}.  In particular it
## contains the field @var{funarguments} with the optional arguments to be used
## in the evaluation of @var{fun}.
##
## The optional fifth argument @var{k_vals_in} contains the Runge-Kutta
## evaluations of the previous step to use in a FSAL scheme.
##
## The optional sixth argument @var{t_next} (@code{t_next = t + dt}) specifies
## the end of the integration interval.  The output @var{x_next} s the higher
## order computed solution at time @var{t_next} (local extrapolation is
## performed).
##
## Optionally the functions can also return @var{x_est}, a lower order solution
## for the estimation of the error, and @var{k_vals_out}, a matrix containing
## the Runge-Kutta evaluations to use in a FSAL scheme or for dense output.
##
## @seealso{runge_kutta_45_dorpri}
## @end deftypefn

function [t_next, x_next, x_est, k] = runge_kutta_23 (fun, t, x, dt,
                                                      options = [],
                                                      k_vals = [],
                                                      t_next = t + dt)

  persistent a = [0   0   0;
                  1/2 0   0;
                  0   3/4 0];
  persistent b = [0, 1/2, 3/4, 1];
  persistent c = [2/9, 1/3, 4/9];
  persistent c_prime = [7/24, 1/4, 1/3, 1/8];

  s = t + dt * b;
  cc = dt * c;
  aa = dt * a;
  k = zeros (rows (x), 4);

  if (! isempty (options))  # extra arguments for function evaluator
    args = options.funarguments;
  else
    args = {};
  endif

  if (! isempty (k_vals))    # k values from previous step are passed
    k(:,1) = k_vals(:,end);  # FSAL property
  else
    k(:,1) = feval (fun, t, x, args{:});
  endif

  k(:,2) = feval (fun, s(2), x + k(:,1) * aa(2, 1).', args{:});
  k(:,3) = feval (fun, s(3), x + k(:,2) * aa(3, 2).', args{:});

  ## compute new time and new values for the unknowns
  ## t_next = t + dt;
  x_next = x + k(:,1:3) * cc(:);  # 3rd order approximation

  ## if the estimation of the error is required
  if (nargout >= 3)
    ## new solution to be compared with the previous one
    k(:,4) = feval (fun, t_next, x_next, args{:});
    cc_prime = dt * c_prime;
    x_est = x + k * cc_prime(:);
  endif

endfunction
