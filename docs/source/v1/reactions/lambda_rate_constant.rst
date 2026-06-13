Lambda Rate Constant
====================

Lambda rate constant reactions compute the rate constant from a user-supplied lambda
function of temperature (and optionally pressure):

.. math::

   \ce{X_1 ( + X_2 \dots ) -> Y_1 ( + Y_2 \dots )}

where :math:`\ce{X_n}` are the reactants, and :math:`\ce{Y_n}` are the products of the reaction.

The rate constant is given by the value returned from the supplied ``lambda function`` when
evaluated at the current temperature :math:`T` (in K) and, optionally, pressure :math:`P` (in Pa).

Input data for lambda rate constant reactions have the following format:

.. tab-set::

    .. tab-item:: YAML

        .. code-block:: yaml

            type: LAMBDA_RATE_CONSTANT
            name: foo-lambda
            gas phase: gas
            lambda function: "[](double T, double P) { return 1.2e-5 * exp(-500.0 / T); }"
            reactants:
              - species name: foo
                coefficient: 1.0
            products:
              - species name: bar
                coefficient: 0.5
              - species name: baz
                coefficient: 0.3


    .. tab-item:: JSON

        .. code-block:: json

            {
                "type": "LAMBDA_RATE_CONSTANT",
                "name": "foo-lambda",
                "gas phase": "gas",
                "lambda function": "[](double T, double P) { return 1.2e-5 * exp(-500.0 / T); }",
                "reactants": [
                    {
                        "species name": "foo",
                        "coefficient": 1.0
                    }
                ],
                "products": [
                    {
                        "species name": "bar",
                        "coefficient": 0.5
                    },
                    {
                        "species name": "baz",
                        "coefficient": 0.3
                    }
                ]
            }

The key-value pairs ``reactants``, ``products``, and ``lambda function`` are required.
Any number of reactants and products may be present. Reactants and products without a specified ``coefficient`` are
assumed to have a ``coefficient`` of 1.0.

The ``lambda function`` is a string holding a function that takes temperature :math:`T` and,
optionally, pressure :math:`P` and returns the rate constant. It is expected to be compiled
by a conforming implementation.

The ``gas phase`` key is required and must be set to the name of the phase the
reaction takes place in. Each reactant must be present in the specified phase.

Rate constants are in units of :math:`\mathrm{(m^{3}\ mol^{-1})^{(n-1)}\ s^{-1}}` where :math:`n` is the total number of reactants.
