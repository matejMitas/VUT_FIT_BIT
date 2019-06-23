import React, {Component} from 'react';
import {Redirect} from "react-router-dom";
import axios from 'axios';
//
import {ROUTE_PREFIX, TOKEN_NAME, LOG_IN, API_BASE, getUser, addToBasket} from '../const.js'

export default class Login extends Component {
    constructor(props) {
        super(props)
        this.state = {
            email               : '',
            password            : '',
            typeOfUser          : 'customer', 
            correctLogin        : false,
            alreadyLogged       : false
        }
        // predana metoda pro kontrolu spravnosti prihlaseni
        this.validLogin = props.onValidLogin
        // 
        this.userSelect = {
            worker      : 'worker',
            customer    : 'customer'
        }
        // this.userForm =

        this.worker = 'worker'
        this.customer = 'customer'
    }

    componentDidMount() {
        //pokud je jiz uzivatel prihlasen a zada manualne stranku loginu, je presmerovan zpet
        if (getUser()) {
            this.setState({correctLogin: true})
        }
    }

    handleChange = (type, event) => {
        let obj = {}
        obj[type] = event.target.value
        this.setState(obj)
    }

    handleRadioChange = (event) => {
        this.setState({typeOfUser: event.target.value})
    }

    resetLoginForm() {
        this.setState({
            email       : '',
            password    : '',
        })
    }

    handleSubmit = async (event) => {   
        event.preventDefault();
        try {
            var response = await axios({
                method: 'post',
                url: `${API_BASE}login`,
                // withCredentials: true,
                data: {
                    email: this.state.email,
                    heslo: this.state.password
                }
            })

            //console.log(response)

            // volba uzivatelskych opravneni
            if (response.data.level > 3 && this.state.typeOfUser !== this.userSelect.worker) {
                this.resetLoginForm()
                alert('Údaje jsou správné, ale pro uživatele, změňte volbu')
            } else if (response.data.level < 3 && this.state.typeOfUser !== this.userSelect.customer) {
                this.resetLoginForm()
                alert('Údaje jsou správné, ale pro pracovníka, změňte volbu')
            } else {
                let token = JSON.stringify({
                    'token'     : response.data.token,
                    'name'      : response.data.name,
                    'email'     : response.data.email,
                    'address'   : response.data.address,
                    'level'     : response.data.level
                })
                // pridano do kosiku z API
                if (response.data.order !== -1) {
                    addToBasket(response.data.order, response.data.email)
                }
                //
                this.props.onHandleAuth(LOG_IN, token)
                localStorage.setItem(TOKEN_NAME, token)
                this.setState({correctLogin: true})
            }
        } catch (error) {
            this.resetLoginForm()
            alert('Špatně vyplněné údaje přihlašovací údaje')
        }
    }

    render() {

        if (this.state.correctLogin) {
            return <Redirect to={`${ROUTE_PREFIX}caje`}/>
        }

        return (
            <div className="login layout__inner">
                <section className="login__customer">
                    <h1>Vstup do systému</h1>
                    <small>Vyberte si prosím vaši roli, vyplňte políčka a zmáčkněte tlačítko přihlásit.</small>
                    <form onSubmit={this.handleSubmit}>

                        <input 
                            type="radio" 
                            id="login_customer" 
                            value={this.userSelect.customer}
                            checked={this.state.typeOfUser === this.userSelect.customer} 
                            onChange={this.handleRadioChange}/>
                        <label className="login__radio" htmlFor="login_customer">Zákazník</label>
                        
                        <input 
                            type="radio" 
                            id="login_worker" 
                            value={this.userSelect.worker}
                            checked={this.state.typeOfUser === this.userSelect.worker} 
                            onChange={this.handleRadioChange}/>
                        <label className="login__radio" htmlFor="login_worker">Pracovník</label>

                        <label>
                            <span>E-mail</span>
                            <input 
                                type="email" 
                                required 
                                value={this.state.email} 
                                onChange={(event) => this.handleChange('email', event)} 
                            />
                        </label>
                        <label>
                            <span>Heslo</span>
                            <input 
                                type="password" 
                                required 
                                value={this.state.password} onChange={(event) => this.handleChange('password', event)} 
                            />
                        </label>
                        <input type="submit" value="Přihlásit se" className="button" />
                    </form>
                </section>
            </div>
        )
    }
}